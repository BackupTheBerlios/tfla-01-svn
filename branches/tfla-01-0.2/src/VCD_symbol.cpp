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
 * Class for handling wire/signal attributes
 */

#include <string.h>
#include <stdio.h>
#include "VCD_symbol.h"

VCD_symbol::VCD_symbol()
{
	name 	= NULL;
	id 		= NULL;
	value 	= NULL;
	wire_nr = -1;
	size = 0;
}

VCD_symbol::~VCD_symbol()
{
	if (name != NULL)
	{
		delete name;
		name = NULL;
	}
	if (id != NULL)
	{
		delete id;
		id = NULL;
	}
	if (value != NULL)
	{
		delete value;
		value = NULL;
	}
}

// return 0 if id already exist 
int VCD_symbol::SetId(const char *new_id) 
{
	unsigned k;
	k = strlen(new_id);
	
	if (id != NULL)
	{
		if (strcmp(new_id,id)==0)
		{
			return(0);
		}
		
		delete(id);
		id=NULL; 
	}
	if (k>0)
	{
		id = new char [k+1];
		strcpy(id,new_id);
	}
	return(k);
} 

// if name already exist do rename 
void VCD_symbol::SetName(const char *new_name) 
{
	unsigned k;
	k = strlen(new_name)+1;
	if (name != NULL)
	{
		delete(name);
	}
	name = new char [k];
	memcpy(name,new_name,k);
} 

void VCD_symbol::SetValue(const char *new_value)
{
	unsigned k,i;
	k = strlen(new_value);
	if (value == NULL)
	{
		value_max_len = k+16;
		value = new char [value_max_len];	
	}
	else if (k+1 > value_max_len)
	{
		delete value;
		value_max_len = k+16;
		value = new char [value_max_len];	
	}
	else if (k == value_len) 
	{
		for (i=0; i<k; i+=1)
		{
			if (value[i] != new_value[i])
			{
				break;
			}
		}
		if (k==i)
		{
			return; // no changes
		}
	}
	memcpy(value,new_value,k+1);
	value_len = k;
	// Verilogic value convesrions
	switch(value[0])
	{ 
		case '0':
			val_i = 0;
			val_r = 0;
		break;
		
		case '1':
			val_i = 1;
			val_r = 1;
		break;
		
		case 'b':
		break;
		
		case 'x':
			val_i = -1;
		break;
		
		case 'z':
			val_i = -1;
			val_r = -1;
		break;
		
		case 'R':
		case 'r':
			sscanf(&value[1],"%lg",&val_r);
		break;
	}
} 
