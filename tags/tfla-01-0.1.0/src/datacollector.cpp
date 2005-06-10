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
#include <qtimer.h>

#include "datacollector.h"
#include "hardware/parportlist.h"

// -------------------------------------------------------------------------------------------------
const int DataCollector::INITIAL_VECTOR_SPACE   = 1000000;

// -------------------------------------------------------------------------------------------------
DataCollector::DataCollector(uint port)
    throw ()
    : m_portNumber(port), m_triggering(false), m_triggeringMask(0), m_triggeringValue(0),
      m_collectingTime(0), m_stop(false)
{
    m_data.bytes().reserve(INITIAL_VECTOR_SPACE);
}

// -------------------------------------------------------------------------------------------------
void DataCollector::setTriggering(bool enabled, unsigned char value, unsigned char mask)
    throw ()
{
    m_triggering = enabled;
    m_triggeringValue = value;
    m_triggeringMask = mask;
}


// -------------------------------------------------------------------------------------------------
bool DataCollector::isTriggeringEnabled() const
    throw ()
{
    return m_triggering;
}


// -------------------------------------------------------------------------------------------------
unsigned char DataCollector::getTriggeringValue() const
    throw ()
{
    return m_triggeringValue;
}


// -------------------------------------------------------------------------------------------------
unsigned char DataCollector::getTriggeringMask() const
    throw ()
{
    return m_triggeringMask;
}


// -------------------------------------------------------------------------------------------------
void DataCollector::setCollectingTime(unsigned int time)
    throw ()
{
    m_collectingTime = time;
}


// -------------------------------------------------------------------------------------------------
unsigned int DataCollector::getCollectingTime() const
    throw ()
{
    return m_collectingTime;
}


// -------------------------------------------------------------------------------------------------
Data DataCollector::getData() const
    throw ()
{
    return m_data;
}


// -------------------------------------------------------------------------------------------------
QString DataCollector::getErrorString() const
    throw ()
{
    return m_errorString;
}


// -------------------------------------------------------------------------------------------------
void DataCollector::stop() 
    throw ()
{
    m_stop = true;
}


// -------------------------------------------------------------------------------------------------
void DataCollector::run()
{
    try
    {
        // create a timer
        std::auto_ptr<QTimer> timer(new QTimer(NULL));
        
        // open the port and claim it
        ParportList* list = ParportList::findPorts();
        Parport port = *(list->getPort(m_portNumber));
        
        // I tried F1284_EXCL here, but if I set this, claim failed with a system error
        port.open(0);
        delete list;
        port.claim();
        port.setDataDirection(true);
        
        // wait for triggering
        if (m_triggering && m_triggeringMask != 0xff)
        {
            PRINT_TRACE("Triggering enabled, starting Parport::waitData()");
            
            qDebug("Mask=%x, Value=%x", m_triggeringMask, m_triggeringValue);
            
            struct timeval timeout = { 0, 500 };
            
            while (!m_stop)
            {
                if (port.waitData(m_triggeringMask, m_triggeringValue, &timeout))
                {
                    break;
                }
            }
        }
        
        timer->start(m_collectingTime, true);
        
        std::vector<byte> vec;
        
        while (timer->isActive() && !m_stop)
        {
            m_data.bytes().push_back(port.readData());
        }
        
        m_data.setMeasuringTime(m_collectingTime);
        
        PRINT_TRACE("Getting data finished with %d samples.", vec.size());
    }
    catch (const TfError& err)
    {
        PRINT_TRACE("Error occured while getting data: %s", (const char*)err.what().local8Bit());
        perror("Test\n");
        m_errorString = err.what();
    }
    
    PRINT_TRACE("Getting data finished with %d samples.", m_data.bytes().count());
}


