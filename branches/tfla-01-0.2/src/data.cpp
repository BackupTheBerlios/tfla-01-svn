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
/*
 * Change log
 * Nov 12 2007 A.Kass General change way for stored samples
 * Samples are stored with time markes only if new value is received
 * Each sample have a time-step marker/counter unil next change of state
 * It's dramaticaly reduce amount of memmory and increse perfomance
 * acces to samples may be optimized for speed
 */
 

#include <qobject.h>
#include "data.h"


#define bit_is_set(b, i) \
     (b & (1 << (i)))

#define bit_is_clear(b, i) \
     (!bit_is_set(b, i))


// -------------------------------------------------------------------------------------------------
Data::Data() throw ()
    : m_measuringTime(0)
{
    for (int i = 0; i < NUMBER_OF_WIRE_PER_SAPMPLE; i++)
    {
        m_lineStates[i] = LS_CHANGING;
    }
    m_start_time = m_stop_time =0;
    m_last_index = 0;
    num_events =0;
    num_samples=0;
}

void Data:: Open(unsigned range_vector)
{
	m_samples.reserve(range_vector);
	if (!m_samples.empty())
	{
		m_samples.clear();
	}
    num_events =0;
    num_samples=0;
}

void Data:: AddSample(unsigned value, unsigned rep_counter)
{
	SampleHold vc;
	vc.sample_time = rep_counter;
	vc.value = value;
	num_events = m_samples.size();
	if (num_events)
	{
		vc.sample_time += m_samples.back().sample_time;
		num_events +=1;
		num_samples = vc.sample_time;
	}
	else
	{
		m_start_time = 0;
		m_stop_time  = rep_counter;
		m_current_value = value;
		m_last_index =1;
		num_events = 1;
		num_samples = rep_counter;
	}
   	m_samples.append(vc);
	
}

// -------------------------------------------------------------------------------------------------
double Data::getMeasuringTimeS() const throw ()
{
    return m_measuringTime;
}

// -------------------------------------------------------------------------------------------------
double Data::getTimeStep() const throw ()
{
    return m_sample_time;
}


// -------------------------------------------------------------------------------------------------
void Data::setMeasuringTime(uint time) throw ()
{
    if (time == 0 || num_samples < 2)
    {
    	m_sample_time = 1E-9;
    	m_measuringTime = 0;
    }
    else
    {
    	m_measuringTime = 0.001;
    	m_measuringTime *= time;
    	m_sample_time = m_measuringTime/num_samples;
    }
}
void Data::setSampleTime(double time)
{
	if (time <= 0)
	{
		m_sample_time = 1E9;
	}
	else
	{
		m_sample_time = time;
	}
	m_measuringTime = num_samples * m_sample_time;
}


// -------------------------------------------------------------------------------------------------
Data::LineState Data::getLineState(int line) const throw ()
{
    return m_lineStates[line];
}

// -------------------------------------------------------------------------------------------------
sample_time_t  Data::getNumSamples(void) const
{
    return num_samples;
}

unsigned Data::GetSampleValue(sample_time_t time_pos)
{
    if (num_events)
    {
		if (time_pos >= m_start_time && time_pos < m_stop_time)
		{
			return(m_current_value);
		}
		if (time_pos >= m_samples.last().sample_time)
		{
			return (m_samples.last().value);
		}
		if (time_pos < m_samples.first().sample_time)
		{
			m_last_index=0;
        	m_start_time=0;
        	m_stop_time  = m_samples.first().sample_time;
            m_current_value = m_samples.first().value;
			return(m_current_value);
		}
		if (time_pos >= m_samples[m_last_index].sample_time)
		{
			do
			{
				m_start_time = m_samples[m_last_index++].sample_time;
        		m_stop_time  = m_samples[m_last_index].sample_time;
			} while (time_pos>=m_stop_time);
        	m_current_value = m_samples[m_last_index].value;
		}
		else
		{
			do
			{
				m_stop_time  = m_samples[m_last_index--].sample_time;
				m_start_time = m_samples[m_last_index].sample_time;
			} while (time_pos<m_start_time);
			m_last_index +=1;
        	m_current_value = m_samples[m_last_index].value;
		}		
		return(m_current_value);
    }
    return(0);	
}

bool Data:: GetWire(sample_time_t time_pos,unsigned nr_wire)
{
	return( GetSampleValue(time_pos) & (1<<nr_wire));	
}

int	 Data:: ScanWire(sample_time_t time_start,sample_time_t delta_time, unsigned nr_wire)
{
	bool c_value;
	int change_count;
	bool vn;
	unsigned mask;
	sample_time_t kt , k_last;
	kt      = time_start;
	k_last  = kt+delta_time;

	if (kt >= num_samples)
	{
		return(0);
	}
	else if (kt < 0)
	{
		if (k_last < 1)
		{
			return(0);
		}
		kt = 0;
	}
	if (k_last > num_samples)
	{
		// protected to search to far
		k_last = num_samples;
	}
	c_value = GetWire(time_start,nr_wire);
	change_count = 0;
	mask = (1<<nr_wire);
	while (kt < k_last)
	{
		vn = (GetSampleValue(kt) & mask);
		if (vn != c_value)
		{
			c_value = !c_value;
			change_count +=1;
		}
		if (kt >= m_start_time && kt < m_stop_time)
		{
			kt = m_stop_time;
		}
		else
		{
			kt +=1;
		}
	}
	return(change_count);
}
// -------------------------------------------------------------------------------------------------
void Data::calculateLineStates() throw ()
{
    unsigned vs;
    if (num_events < 1)
    {
        vs =0;
    }
    else
    {
    	vs = m_samples[0].value;
    }

    for (int i = 0; i < NUMBER_OF_WIRE_PER_SAPMPLE; i++)
    {
        m_lineStates[i] = bit_is_set(vs,i) ? LS_ALWAYS_H : LS_ALWAYS_L;
    }
            
    for (uint j = 1; j < num_events; j++)
    {
    	vs = m_samples[j].value;
    	for (int i = 0; i < NUMBER_OF_WIRE_PER_SAPMPLE; i++)
    	{
            switch (m_lineStates[i])
            {
                case LS_ALWAYS_L:
                    if (bit_is_set(vs, i))
                    {
                        m_lineStates[i] = LS_CHANGING;
                    }
                    break;
                    
                case LS_ALWAYS_H:
                    if (bit_is_clear(vs, i))
                    {
                        m_lineStates[i]  = LS_CHANGING;
                    }
                    break;
                    
                case LS_CHANGING:
                    // don't occure
                    break;
            }
        }
    }
}


// -------------------------------------------------------------------------------------------------
double Data::getMsecsForSample(long long sample) const throw()
{
    if (sample == -1)
    {
        // special case, the invalid position
        return -1.0;
    }
    else if (sample < 0 || sample >= (int)num_samples )
    {
        return -1.0;
    }
    
    return sample * m_sample_time*1000;
    
}
