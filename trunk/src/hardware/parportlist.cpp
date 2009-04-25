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
#include <ieee1284.h>

#include "hardware/parportlist.h"

// -------------------------------------------------------------------------------------------------
ParportList* ParportList::m_theInstance = 0;


// -------------------------------------------------------------------------------------------------
ParportList* ParportList::instance(int flags) throw (ParportError)
{
    if (!m_theInstance) {
        parport_list* list = new parport_list;
        int err;

        if ((err = ieee1284_find_ports(list, flags)) != E1284_OK) {
            delete list;
            throw ParportError(err);
        }

        m_theInstance = new ParportList(list);
    }

    return m_theInstance;
}


// -------------------------------------------------------------------------------------------------
ParportList::ParportList(struct parport_list* list) throw ()
{
    m_list = list;
}

// -------------------------------------------------------------------------------------------------
ParportList::~ParportList()
{
    ieee1284_free_ports(m_list);
    delete m_list;
}


// -------------------------------------------------------------------------------------------------
int ParportList::getNumberOfPorts() const throw ()
{
    return m_list->portc;
}


// -------------------------------------------------------------------------------------------------
Parport ParportList::getPort(int number) const throw ()
{
    return Parport(m_list->portv[number]);
}

// vim: set sw=4 ts=4 tw=100:
