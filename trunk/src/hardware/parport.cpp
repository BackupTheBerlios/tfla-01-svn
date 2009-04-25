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

#ifdef __linux__
#  include <linux/ppdev.h>
#  include <linux/parport.h>
#  include <unistd.h>
#  include <sys/ioctl.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <sys/time.h>
#  include <fcntl.h>
#endif

// -------------------------------------------------------------------------------------------------
Parport::Parport(struct parport* port)
    : m_isOpen(false), m_isClaimed(false)
{
    m_parport = port;
}


// -------------------------------------------------------------------------------------------------
Parport::~Parport()
{
    if (m_isOpen) {
        try {
            close();
        } catch (const TfError& err) {
            PRINT_DBG("Error in Parport::~Parport: %s\n", (const char*)err.what().local8Bit());
        }
    }
}


// -------------------------------------------------------------------------------------------------
void Parport::open(int flags, int* capabilities) throw (ParportError)
{
    int ret;

    // that check must be performed before opening the device
    if (!checkTristate())
        throw ParportError(ParportError::EPP_NOTRISTATE);

    if ((ret = ieee1284_open(m_parport, flags, capabilities)) != E1284_OK)
        throw ParportError(ret);

    m_isOpen = true;
}


// -------------------------------------------------------------------------------------------------
void Parport::close() throw (ParportError)
{
    if (!m_isOpen)
        return;

    release();

    int ret;

    if ((ret = ieee1284_close(m_parport)) != E1284_OK)
        throw ParportError(ret);
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
        throw ParportError(err);

    m_isClaimed = true;
}


// -------------------------------------------------------------------------------------------------
#ifdef __linux__
bool Parport::checkTristate() throw ()
{
    int modes;

    // only return false if we *know* that the parallel port is not capable of TRISTATE
    if (!m_parport->filename || strlen(m_parport->filename) == 0)
        return true;

    int fd = ::open(m_parport->filename, O_RDONLY);
    if (fd < 0)
        return true;

    if (ioctl(fd, PPCLAIM) != 0) {
        ::close(fd);
        return true;
    }

    if (ioctl(fd, PPGETMODES, &modes) != 0) {
        ::close(fd);
        return true;
    }

    ::close(fd);

    if (!(modes & PARPORT_MODE_TRISTATE))
        return false;

    return true;
}
#else /* !__linux__ */
bool Parport::checkTristate() throw ()
{
    return true;
}
#endif /* __linux__ */

// -------------------------------------------------------------------------------------------------
byte Parport::readData() throw (ParportError)
{
    int ret;

    if ((ret = ieee1284_read_data(m_parport)) < 0)
        throw ParportError(ret);

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
        throw ParportError(ret);
}


// -------------------------------------------------------------------------------------------------
bool Parport::waitData(int mask, int val, struct timeval* timeout, bool poll)
    throw (ParportError)
{
    if (poll)
        return waitDataPoll(mask, val, timeout);
    else
        return waitDataIeee1284(mask, val, timeout);
}

// -------------------------------------------------------------------------------------------------
bool Parport::waitDataPoll(int mask, int val, struct timeval* timeout)
    throw (ParportError)
{
#ifndef __linux__
    // don't know about timing operations on Windows, but don't also want to
    // break the Build on Unix
    return waitDataIeee1284(mask, val, timeout);
#else

    struct timeval now;
    struct timeval timeout_reached;

    // check if we need to wait for data
    if (mask == 0)
        return true;

    int ret = gettimeofday(&now, NULL);
    if (ret != 0) {
        perror("gettimeofday() failed");
        return false;
    }

    timeradd(&now, timeout, &timeout_reached);

    do {
        byte data = readData();

        if ((data & mask) == (val & 0xff))
            return true;

        ret = gettimeofday(&now, NULL);
        if (ret != 0) {
            perror("gettimeofday() failed");
            return false;
        }

    } while (timercmp(&now, &timeout_reached, <));


    // timeout reached
    return false;
#endif
}

// -------------------------------------------------------------------------------------------------
bool Parport::waitDataIeee1284(int mask, int val, struct timeval* timeout)
    throw (ParportError)
{
    int ret;
    struct timeval default_timeout = { 60, 0 };

    if (!timeout)
        timeout = &default_timeout;

    if ((ret = ieee1284_wait_data(m_parport, mask, val, timeout)) != E1284_OK) {
        if (ret == E1284_TIMEDOUT)
            return false;
        else
            throw ParportError(ret);
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

// vim: set sw=4 ts=4 tw=100:
