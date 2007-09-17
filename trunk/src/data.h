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
#ifndef DATA_H
#define DATA_H

#include <qmainwindow.h>
#include <qaction.h>

#include "global.h"

class Data
{
    public:
        enum LineState { LS_ALWAYS_L, LS_ALWAYS_H, LS_CHANGING };
        
    public:
        Data() throw ();
        
        ByteVector& bytes() throw();
        const ByteVector& bytes() const throw();
        
        double getMsecsForSample(int sample) const throw();
        
        uint getMeasuringTime() const throw ();
        void setMeasuringTime(uint time) throw ();
        
        LineState getLineState(int line) const throw ();
        void calculateLineStates() throw ();
        
    private:
        ByteVector m_bytes;
        uint       m_measuringTime;
        LineState  m_lineStates[NUMBER_OF_BITS_PER_BYTE];
};

#define bit_is_set(b, i) \
     (b & (1 << (i)))

#define bit_is_clear(b, i) \
     (!bit_is_set(b, i))

#endif /* DATA_H */
