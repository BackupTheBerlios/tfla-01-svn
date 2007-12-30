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

#include <qdatetime.h>
#include "global.h"

struct _SampleHold
{
	unsigned int	sample_time;
	unsigned char 	value;
};

typedef _SampleHold SampleHold;
/**
 * ByteVector is shorter than QValueVector\<byte\> or even QValueVector\<unsigned char\>.
 */
typedef QValueVector<SampleHold> SampleVector;

#define DATA_MAX_WIRE_SLOTS 24

class Data
{
    public:
        enum LineState { LS_ALWAYS_L, LS_ALWAYS_H, LS_CHANGING };
        
    public:
        Data() throw ();
               
        double getMsecsForSample(long long sample) const throw();
        sample_time_t getNumSamples(void) const ;
        bool     DataLoaded(void) { return (num_samples > 1); }
        
        double getMeasuringTimeS() const throw ();
        double getTimeStep() const throw ();

        void setMeasuringTime(uint time_ms) throw ();	/* round to msec */
        void setSampleTime(double step);
        
        LineState getLineState(int line) const throw ();
        void calculateLineStates() throw ();
        
        QDateTime     m_StartTime;
        unsigned GetSampleValue(sample_time_t time_pos);
        bool	 GetWire(sample_time_t time_pos,unsigned nr_wire);
        int	 	 ScanWire(sample_time_t time_start,sample_time_t num_samples, unsigned nr_wire);
        void	 AddSample(unsigned value, unsigned rep_counter);
        void     Open(unsigned vector_range);
    private:
        SampleVector m_samples;
        double       m_measuringTime;
        double		 m_sample_time;
        LineState    m_lineStates[DATA_MAX_WIRE_SLOTS];
        unsigned	 m_last_index;
        unsigned     m_start_time;
        unsigned	 m_stop_time;
        unsigned	 m_current_value;
        unsigned	 num_events;
        unsigned	 num_samples;
};

#endif /* DATA_H */
