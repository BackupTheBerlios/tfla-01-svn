/* 
 * Copyright (c) 2007, Andrzej Kass
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
 * Dummy implementation of iee1284 lib's features for Windows_XP os to be filled 
 * -------------------------------------------------------------------------------------------------
 */
#include "parport.h"
#include "global.h"
#include <ieee1284.h>


extern "C" 
{

int ieee1284_find_ports (struct parport_list *list, int flags)
{
	list->portc=0;
	list->portv=NULL;
	return(E1284_OK);
	
}

void ieee1284_free_ports(struct parport_list *list)
{
	if (list != NULL && list->portc >0)
	{
		while(list->portc)
		{
			list->portc -=1;
		}
	}
}



int ieee1284_open (struct parport *port, int flags, int *capabilities)
{
	return(E1284_OK);
}

int ieee1284_close (struct parport *port)
{
	return(E1284_OK);
}


//extern int ieee1284_ref (struct parport *port);
//extern int ieee1284_unref (struct parport *port);

int ieee1284_claim (struct parport *port)
{
	return(E1284_OK);
}
/* Must be called before any function below.  May fail. */

void ieee1284_release (struct parport *port)
{
}


int ieee1284_read_data (struct parport *port)
{
	return(0);
}

void ieee1284_write_data (struct parport *port, unsigned char dt)
{
}

int ieee1284_wait_data (struct parport *port, unsigned char mask, unsigned char val, struct timeval *timeout)
{
	return(0);
}

int ieee1284_data_dir (struct parport *port, int reverse)
{
	return(E1284_OK);
}



}

