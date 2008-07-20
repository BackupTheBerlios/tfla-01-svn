/*
 * Copyright (c) 2005, Bernhard Walle
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; You may only use
 * version 2 of the License, you have no option to use any other version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * -------------------------------------------------------------------------------------------------
 */
//#include <sys/timeb.h>

#include <errno.h>
#include <cstdlib>

#include <vector>

#include <qobject.h>
#include <qdatetime.h>
#include <qcstring.h>

#include "datacollector.h"
#include "hardware/parportlist.h"

using std::free;

// -------------------------------------------------------------------------------------------------
const int DataCollector::INITIAL_VECTOR_SPACE   = 1000000;

// -------------------------------------------------------------------------------------------------
DataCollector::DataCollector(uint port) throw ()
    : m_portNumber(port), m_triggering(false), m_triggeringMask(0), m_triggeringValue(0),
      m_collectingTime(0), m_stop(false), m_numberOfSkips(0)
{
    m_data.Open(INITIAL_VECTOR_SPACE);
}

// -------------------------------------------------------------------------------------------------
void DataCollector::setTriggering(bool enabled, unsigned char value, unsigned char mask) throw ()
{
    m_triggering = enabled;
    m_triggeringValue = value;
    if (mask == 0)
    {
    	m_triggering = false;
    }
    m_triggeringMask = mask;
}


// -------------------------------------------------------------------------------------------------
bool DataCollector::isTriggeringEnabled() const throw ()
{
    return m_triggering;
}


// -------------------------------------------------------------------------------------------------
unsigned DataCollector::getTriggeringValue() const throw ()
{
    return m_triggeringValue;
}


// -------------------------------------------------------------------------------------------------
unsigned DataCollector::getTriggeringMask() const throw ()
{
    return m_triggeringMask;
}


// -------------------------------------------------------------------------------------------------
void DataCollector::setCollectingTime(int time) throw ()
{
    m_collectingTime = time;
}


// -------------------------------------------------------------------------------------------------
int DataCollector::getCollectingTime() const throw ()
{
    return m_collectingTime;
}


// -------------------------------------------------------------------------------------------------
void DataCollector::setNumberOfSkips(int numberOfSkips) throw ()
{
    m_numberOfSkips = numberOfSkips;
}


// -------------------------------------------------------------------------------------------------
int DataCollector::getNumberOfSkips() const throw ()
{
    return m_numberOfSkips;
}


// -------------------------------------------------------------------------------------------------
Data DataCollector::getData() const throw ()
{
    return m_data;
}


// -------------------------------------------------------------------------------------------------
QString DataCollector::getErrorString() const throw ()
{
    return m_errorString;
}


// -------------------------------------------------------------------------------------------------
void DataCollector::stop()  throw ()
{
    m_stop = true;
}

#define MEGA_SAMPLE_NUM (1024*256)

// -------------------------------------------------------------------------------------------------
void DataCollector::run()
{
    unsigned      *sample_rep=NULL;
    unsigned char *sample_val=NULL;
    try
    {
        // open the port and claim it
        ParportList* list = ParportList::instance();
        Parport port = list->getPort(m_portNumber);
        unsigned  z_sample_pos;
        unsigned  z_sample_num;
        unsigned  ns;
        unsigned  aa,bb;
        unsigned  aa_counter;

		m_collectingTime += 50;

		int loop_counter = m_collectingTime/20;



		z_sample_num = MEGA_SAMPLE_NUM;
		sample_rep = (unsigned *)malloc((sizeof(unsigned)+sizeof(char))*z_sample_num);

		if (sample_rep == NULL)
		{
			return;
		}
		sample_val = (unsigned char *)&sample_rep[z_sample_num];
    	for (z_sample_pos=0; z_sample_pos < z_sample_num ; z_sample_pos +=1)
    	{
    		sample_rep[z_sample_pos]=0;
    		sample_val[z_sample_pos]=0;
    	}
        z_sample_pos = 0;


        // I tried F1284_EXCL here, but if I set this, claim failed with a system error
        port.open(0);
        port.claim();
        port.setDataDirection(true);

        // wait for triggering

        if (m_triggering && m_triggeringMask != 0xff)
        {


            while (!m_stop)
            {
                if (port.waitData(m_triggeringMask, m_triggeringValue, 500))
                {
                    break;
                }
            }
        }

        // we cannot use QTimer in this thread because it's in non-GUI thread
        // this changes in Qt 4, but this is future
        // so we need to poll, and to be platform independent we use QTime
        // QTime::msecsTo(QTime::currentTime) isn't very fast, so we do this every 100 measures
        // which seems to be accurate enough
        z_sample_pos = 0;
        unsigned      *v_counter = sample_rep;
        unsigned char *v_samples = sample_val;


        QTime start = QTime::currentTime();

		aa=port.readData();
	    aa_counter=1;
        do
        {
			// 2us for sample
			// 100ms/blok = 5
            for (int i = 0; i < 10*500; i++)
            {
                for (int j = 0; j < m_numberOfSkips; j++)
                {
                    port.readData();
                }
//                yield();
//                sched_yield();
                bb  = port.readData();
              	if ((aa == bb)  && (aa_counter < 0xFFFFFF00))
              	{
                	aa_counter +=1;
              		continue;
              	}
	          	*v_counter++ = aa_counter;
	            *v_samples++ = aa;
         		aa = bb;
         		aa_counter =1;
           		z_sample_pos++;
           		if (z_sample_pos+1 == z_sample_num)
              	{
              		m_stop = true;
	            	break;
              	}
            }
			loop_counter-=1;
        } while (!m_stop && loop_counter>0);

		*v_samples = aa;
	    *v_counter = aa_counter;
        z_sample_pos+=1;

        unsigned run_time = start.msecsTo(QTime::currentTime());
        v_counter = sample_rep;
        v_samples = sample_val;

    	QDateTime  dt = QDateTime::currentDateTime();
    	dt.setTime(start);
		for (ns=0; ns<z_sample_pos; ns+=1)
		{
			m_data.AddSample(*v_samples,*v_counter);
			v_samples +=1;
			v_counter +=1;
		}
        free(sample_rep);
        m_data.m_StartTime = dt;
        m_data.setMeasuringTime(run_time);
        m_data.calculateLineStates();
    }
    catch (const TfError& err)
    {
        if (sample_rep != NULL)
        {
        	free(sample_rep);
        }
        PRINT_TRACE("Error occured while getting data: %s", (const char*)err.what().local8Bit());
        PRINT_DBG("OS error was %s", strerror(errno));
        m_errorString = err.what();
    }
}

