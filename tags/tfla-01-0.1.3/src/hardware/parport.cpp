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
#include "parport.h"
#include "global.h"

// -------------------------------------------------------------------------------------------------
Parport::Parport(struct parport* port)
    : m_isOpen(false), m_isClaimed(false)
{
    m_parport = port;
}


// -------------------------------------------------------------------------------------------------
Parport::~Parport()
{
    if (m_isOpen)
    {
        try
        {
            close();
        }
        catch (const TfError& err)
        {
            PRINT_DBG("Error in Parport::~Parport: %s\n", (const char*)err.what().local8Bit());
        }
    }
}


// -------------------------------------------------------------------------------------------------
void Parport::open(int flags, int* capabilities) throw (ParportError)
{
    int ret;
    
    if ((ret = ieee1284_open(m_parport, flags, capabilities)) != E1284_OK)
    {
        throw ParportError(ret);
    }
    
    m_isOpen = true;
}


// -------------------------------------------------------------------------------------------------
void Parport::close() throw (ParportError)
{
    if (!m_isOpen)
    {
        return;
    }
    
    release();
    
    int ret;
    
    if ((ret = ieee1284_close(m_parport)) != E1284_OK)
    {
        throw ParportError(ret);
    }
}


// -------------------------------------------------------------------------------------------------
void Parport::release() throw ()
{
    ieee1284_release(m_parport);
    
    m_isClaimed = false;
}


// -------------------------------------------------------------------------------------------------
void Parport::claim() throw (ParportError)
{
    int err;
    
    if ((err = ieee1284_claim(m_parport)) != E1284_OK)
    {
        throw ParportError(err);
    }
    
    m_isClaimed = true;
}


// -------------------------------------------------------------------------------------------------
byte Parport::readData() throw (ParportError)
{
    int ret;
    
    if ((ret = ieee1284_read_data(m_parport)) < 0)
    {
        throw ParportError(ret);
    }
    
    return (byte)ret;
}


// -------------------------------------------------------------------------------------------------
void Parport::writeData(byte data) throw ()
{
    ieee1284_write_data(m_parport, data);
}


// -------------------------------------------------------------------------------------------------
void Parport::setDataDirection(bool reverse) throw (ParportError)
{
    int ret;
    
    if ((ret = ieee1284_data_dir(m_parport, reverse)) != E1284_OK)
    {
        throw ParportError(ret);
    }
}


// -------------------------------------------------------------------------------------------------
bool Parport::waitData(int mask, int val, struct timeval* timeout) throw (ParportError)
{
    int ret;
    
    if ((ret = ieee1284_wait_data(m_parport, mask, val, timeout)) != E1284_OK)
    {
        if (ret == E1284_TIMEDOUT)
        {
            return false;
        }
        else
        {
            throw ParportError(ret);
        }
    }
    
    return true;
}
        


// -------------------------------------------------------------------------------------------------
QString Parport::getName() const throw ()
{
    return QString::fromLocal8Bit(m_parport->name);
}


// -------------------------------------------------------------------------------------------------
unsigned long Parport::getBaseAddress() const throw ()
{
    return m_parport->base_addr;
}


// -------------------------------------------------------------------------------------------------
unsigned long Parport::getHighBaseAddress() const throw ()
{
    return m_parport->hibase_addr;
}


// -------------------------------------------------------------------------------------------------
QString Parport::getFileName() const throw ()
{
    return QString::fromLocal8Bit(m_parport->filename);
}
