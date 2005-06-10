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
#ifndef DATACOLLECTOR_H
#define DATACOLLECTOR_H

#include <qobject.h>
#include <qdatetime.h>
#include <qcstring.h>
#include <qvaluevector.h>
#include <qthread.h>

#include "hardware/parport.h"
#include "global.h"
#include "data.h"

/**
 * This class is to collect the data from the parallel port. The data is collected once, so
 * no update is possible. You set up the object, run the thread and once it finishes the
 * data are available. You may access the data in between, and you may stop collecting immediately.
 */
class DataCollector : public QThread
{
    public:
        static const int INITIAL_VECTOR_SPACE;
    
    public:
        DataCollector(unsigned int portNumber) throw ();
    
        void setTriggering(bool enabled, unsigned char value = 0x00, unsigned char mask = 0x00)
        throw ();
        bool isTriggeringEnabled() const throw ();
        byte getTriggeringValue() const throw ();
        byte getTriggeringMask() const throw ();
        void setCollectingTime(unsigned int msecs) throw ();
        unsigned int getCollectingTime() const throw ();
        Data getData() const throw ();
        QString getErrorString() const throw ();
        
        void stop()  throw ();
        
    protected:
        void run();
    
    private:
        unsigned int  m_portNumber;
        bool          m_triggering;
        byte          m_triggeringMask;
        byte          m_triggeringValue;
        Data          m_data;
        unsigned int  m_collectingTime;
        QString       m_errorString;
        bool          m_stop;
};


#endif /* DATACOLLECTOR_H */
