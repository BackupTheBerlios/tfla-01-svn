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
#ifndef PARPORTERROR_H
#define PARPORTERROR_H

#include <stdexcept>
#include <qstring.h>

#include <tferror.h>

/**
 * Exception class for the parallel port. Uses error codes of libieee1284 and translates them
 * to a well-formatted error message.
 *
 * @author $Author$
 * @version $Rev$
 */
class ParportError : public TfError
{
    public:
        /**
         * Creates a new exception using error codes from libieee1284.
         */
        ParportError(int errorcode)
        throw ();
        
        /**
         * Returns the error message. 
         */
        virtual QString what() const 
        throw ();
        
        /**
         * Returns the error code. 
         */
        int getErrorCode() const
        throw ();
    
    private:
        int m_errorcode;
};

#endif /* PARPORTERROR_H */
