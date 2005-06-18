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
#ifndef PARPORTLIST_H
#define PARPORTLIST_H

#include <ieee1284.h>

#include "hardware/parport.h"

/**
 * Lists all parallel ports available on the system.
 *
 * Important: You first have to open the parallel port you would like to use before you
 * can free this object. There's an internal reference count.
 *
 * @author $Author$
 * @version $Rev$ 
 */
class ParportList
{
    public:
        
        /**
         * Finds ports. See http://cyberelk.net/tim/libieee1284/interface/find-ports.html
         * for details.
         *
         * @param flags see http://cyberelk.net/tim/libieee1284/interface/find-ports.html
         *        relevant only on the first call!
         * @exception ParportError on error
         */
        static ParportList* instance(int flags = 0) throw (ParportError);
    
        /**
         * Frees a ParportList. See class description. 
         */
        virtual ParportList::~ParportList();
        
        /**
         * Returns the number of ports.
         *
         * @return the number of available ports
         */
        int getNumberOfPorts() const throw ();
        
        /**
         * Returns a port with the specified number which must be smaller than getNumberOfPorts().
         *
         * @param number the port number
         */
        Parport getPort(int number) const throw ();
        
    protected:
        
        /**
         * Creates a new ParportList object. The list will be freed by the object themselve, so
         * don't free it from outside.
         *
         * @param list the parport list which is allocated using ParportList::findPorts().
         */
        ParportList(struct parport_list* list) throw ();    
    
    private:
        struct parport_list* m_list;
        static ParportList* m_theInstance;
};

#endif /* PARPORTLIST_H */
