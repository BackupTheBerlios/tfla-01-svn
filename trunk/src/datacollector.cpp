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
#include <sys/timeb.h>

#include <vector>

#include <qobject.h>
#include <qdatetime.h>
#include <qcstring.h>

#include "datacollector.h"
#include "hardware/parportlist.h"

// -------------------------------------------------------------------------------------------------
const int DataCollector::INITIAL_VECTOR_SPACE   = 1000000;

// -------------------------------------------------------------------------------------------------
DataCollector::DataCollector(uint port) throw ()
    : m_portNumber(port), m_triggering(false), m_triggeringMask(0), m_triggeringValue(0),
      m_collectingTime(0), m_stop(false), m_numberOfSkips(0)
{
    m_data.bytes().reserve(INITIAL_VECTOR_SPACE);
}

// -------------------------------------------------------------------------------------------------
void DataCollector::setTriggering(bool enabled, unsigned char value, unsigned char mask) throw ()
{
    m_triggering = enabled;
    m_triggeringValue = value;
    m_triggeringMask = mask;
}


// -------------------------------------------------------------------------------------------------
bool DataCollector::isTriggeringEnabled() const throw ()
{
    return m_triggering;
}


// -------------------------------------------------------------------------------------------------
unsigned char DataCollector::getTriggeringValue() const throw ()
{
    return m_triggeringValue;
}


// -------------------------------------------------------------------------------------------------
unsigned char DataCollector::getTriggeringMask() const throw ()
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


// -------------------------------------------------------------------------------------------------
void DataCollector::run()
{
    try
    {
        // open the port and claim it
        ParportList* list = ParportList::instance();
        Parport port = list->getPort(m_portNumber);
        
        // I tried F1284_EXCL here, but if I set this, claim failed with a system error
        port.open(0);
        port.claim();
        port.setDataDirection(true);
        
        // wait for triggering
        if (m_triggering && m_triggeringMask != 0xff)
        {
            struct timeval timeout = { 0, 500 };
            
            while (!m_stop)
            {
                if (port.waitData(m_triggeringMask, m_triggeringValue, &timeout))
                {
                    break;
                }
            }
        }
        
        QTime start = QTime::currentTime();
        
        // we cannot use QTimer in this thread because it's in non-GUI thread
        // this changes in Qt 4, but this is future
        // so we need to poll, and to be platform independent we use QTime
        // QTime::msecsTo(QTime::currentTime) isn't very fast, so we do this every 100 measures
        // which seems to be accurate enough
        while ((start.msecsTo(QTime::currentTime()) <= m_collectingTime) && !m_stop)
        {
            for (int i = 0; i < 100 && !m_stop; i++)
            {
                for (int j = 0; j < m_numberOfSkips; j++)
                {
                    port.readData();
                }
                m_data.bytes().push_back(port.readData());
            }
        }
        
        m_data.setMeasuringTime(start.msecsTo(QTime::currentTime()));
        m_data.calculateLineStates();
    }
    catch (const TfError& err)
    {
        PRINT_TRACE("Error occured while getting data: %s", (const char*)err.what().local8Bit());
        PRINT_DBG("OS error was %s", strerror(errno));
        m_errorString = err.what();
    }
}

