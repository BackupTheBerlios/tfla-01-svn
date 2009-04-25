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
#include <qstring.h>
#include <QObject>

#include <ieee1284.h>

#include "parporterror.h"

// -------------------------------------------------------------------------------------------------
ParportError::ParportError(int errorcode)
    throw ()
{
    m_errorcode = errorcode;
}

// -------------------------------------------------------------------------------------------------
QString ParportError::what() const
    throw ()
{
    switch (m_errorcode) {
        case E1284_OK:
            return QObject::tr("No error ocurred.");
        case E1284_INIT:
            return QObject::tr("There was a problem during port initialization. This "
                "could be because another driver has opened the port exclusively, or "
                "some other reason.");
        case E1284_NOMEM:
            return QObject::tr("There is not enough memory.");
        case E1284_NOTAVAIL:
            return QObject::tr("Requested feature not available on this system.");
        case E1284_INVALIDPORT:
            return QObject::tr("The port parameter is invalid (for instance, the port  may "
                "already be open and the program tried to open it)");
        case E1284_SYS:
            return QObject::tr("There was a problem at the operating system level. The global "
                "variable errno  has been set appropriately.");
        case EPP_NOTRISTATE:
            return QObject::tr("The parallel port is not capable of TRISTATE. Read "
                "the documentation to get hints how that can be fixed.");
        default:
            return QObject::tr("Invalid error code.");
    }
}


// -------------------------------------------------------------------------------------------------
int ParportError::getErrorCode() const
    throw ()
{
    return m_errorcode;
}

// vim: set sw=4 ts=4 tw=100:
