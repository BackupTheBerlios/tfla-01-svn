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
#include <qobject.h>

#include "data.h"


// -------------------------------------------------------------------------------------------------
Data::Data() throw ()
    : m_measuringTime(0)
{}


// -------------------------------------------------------------------------------------------------
ByteVector& Data::bytes() throw()
{
    return m_bytes;
}


// -------------------------------------------------------------------------------------------------
const ByteVector& Data::bytes() const throw()
{
    return m_bytes;
}


// -------------------------------------------------------------------------------------------------
uint Data::getMeasuringTime() const throw ()
{
    return m_measuringTime;
}


// -------------------------------------------------------------------------------------------------
void Data::setMeasuringTime(uint time) throw ()
{
    m_measuringTime = time;
}


// -------------------------------------------------------------------------------------------------
double Data::getMsecsForSample(int sample) const throw()
{
    if (sample == -1)
    {
        // special case, the invalid position
        return -1.0;
    }
    else if (sample < 0 || sample > static_cast<int>(m_bytes.size()))
    {
        PRINT_DBG("Sample is invalid (%d), should be between 0 and %d", sample, m_bytes.size());
        return -1.0;
    }
    
    return m_measuringTime * static_cast<double>(sample) / m_bytes.size();
    
}
