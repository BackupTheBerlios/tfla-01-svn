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
#ifndef PARPORT_H
#define PARPORT_H

#include <qstring.h>

#include <ieee1284.h>

#include "global.h"
#include "parporterror.h"

/**
 * Represents the parallel port of the system. Uses libieee1284 internally, so it should
 * work on most operating systems.
 *
 * If your operating system is not supported, the correct way is NOT to implement this class
 * on another operating system but to port ieee1284 to that operating system so that all users
 * of ieee1284 profit!
 *
 * This class is just the bridge from IEEE1284 to Qt (C++).
 *
 * The complete documentation of libieee1284 in the Web at http://cyberelk.net/tim/libieee1284/.
 * You also should have it on your local system if libieee1284 is installed (at least on Linux).
 *
 * @author $Author$
 * @version $Rev$
 */
class Parport
{
    friend class ParportList;

   public:

        /**
         * Deletes a parport object. If the parallel port is open, it will be closed. This
         * greatly simplifies error handling using C++ exceptions.
         */
        virtual ~Parport();

        /**
         * In order to begin using a port it must be opened. Any initial set-up of the port is done
         * at this stage. When an open port is no longer needed it should be closed using
         * close().
         *
         * You'll find the meaning of capabilities and flags in
         * http://cyberelk.net/tim/libieee1284/interface/open.html.
         *
         * @param flags  F1284_EXCL if this device cannot share the port with any other device.
         * @param capabilities @c NULL or a pointer to storage for an int, which will be treated
         *        as a set of flags, one per bit, which the library sets or clears as appropriate.
         * @exception ParportError if the opening failed
         */
        void open(int flags = 0, int* capabilities = NULL)
        throw (ParportError);

        /**
         * Closes the port.
         *
         * @exception ParportError if the closing failed
         */
        void close()
        throw (ParportError);

        /**
         * Claim access to the port. claim() must be called on an open port before any other
         * libieee1284 function for accessing a device on it.
         *
         * @exception ParportError if the closing failed
         */
        void claim()
        throw (ParportError);

        /**
         * This function undoes the effect of claim() by releasing the port for use by other
         * drivers. It is good practice to release the port whenever convenient.
         */
        void release()
        throw ();

        /**
         * Reads data. See http://cyberelk.net/tim/libieee1284/interface/data-pin-access.html.
         *
         * @return the read data value
         * @exception ParportError if an error occured
         */
        byte readData()
        throw (ParportError);

        /**
         * Writes data to the data pins. See
         * http://cyberelk.net/tim/libieee1284/interface/data-pin-access.html
         *
         * @param data the data to write
         */
        void writeData(byte data)
        throw ();

        /**
         * Checks if the hardware parallel port is capable of TRISTATE.
         *
         * This function is only implemented on Linux since it does not use libieee1284
         * but raw ppdev access functions.
         *
         * @return @c true if the parallel port is capable of tristate (or if the function
         *         is not implemented, @c false if it is not capable of tristate
         */
        bool checkTristate()
        throw ();

        /**
         * Sets the data direction.
         * See http://cyberelk.net/tim/libieee1284/interface/data-pin-access.html.
         *
         * @param reverse @c true for read mode, @c false for write mode
         * @exception ParportError if an error occured
         */
        void setDataDirection(bool reverse)
        throw (ParportError);

        /**
         * Wait for data.
         * See http://cyberelk.net/tim/libieee1284/interface/data-pin-access.html.
         *
         * @return @c true if wait was successful, @c false if timed out
         * @throw ParportError if an error occured
         */
        bool waitData(int mask, int val, struct timeval* timeout)
        throw (ParportError);

        /**
         * Returns the name of the parallel port.
         *
         * @return the name as string
         */
        QString getName() const
        throw ();

        /**
         * Returns the base address of the parallel port, if this has any meaning, or zero.
         *
         * @return the base address
         */
        unsigned long getBaseAddress() const
        throw ();

        /**
         * Returns the ECR address of the port, if that has any meaning, or zero.
         *
         * @return the address
         */
        unsigned long getHighBaseAddress() const
        throw ();

        /**
         * The filename associated with this port, if that has any meaning, or QString::null.
         *
         * @return the filename, e.g. /dev/parport0 on Linux.
         */
        QString getFileName() const
        throw ();

     protected:

        /**
         * Creates a new instance of a parallel port. This instance is created using a
         * struct parport* pointer from C API. The user don't have to call this function
         * (since it is proteced, he even cannot), but he has to use the ParportList class
         * to get new instances.
         */
        Parport(struct parport* port);

    private:
        struct parport* m_parport;
        bool   m_isOpen;
        bool   m_isClaimed;
};


#endif /* PARPORT_H */

// vim: set sw=4 ts=4 tw=100:
