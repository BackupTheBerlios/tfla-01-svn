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

#ifndef VCD_SYMBOL_H_
#define VCD_SYMBOL_H_


class VCD_symbol
{
	class VCD_symbol *next;
	char *name;		/* wire,line name */
	char *id;		/* wire ID shortm name */
	char *value;	/* Verilogic coded value */
	unsigned value_len;
	unsigned value_max_len;
	double val_r;	/* real   represantation */
	long   val_i;   /* Binary represantation */
public:
	int size;
	unsigned char vartype;
	int wire_nr;
	VCD_symbol();
	const char *Name(void) { return( (name !=NULL) ? name : ""); }
	const char *Id(void)   { return( (id   !=NULL) ? id   : ""); }
	const char *Value(void){ return( (value!=NULL) ? value: ""); }
	
	void   SetName(const char *new_name); 
	int    SetId(const char *new_id);  
	void   SetValue(const char *new_value);
	long   GetValue(void) 	{ return( val_i ); }
	double GetValueR(void)	{ return( val_r ); }
	
	
	VCD_symbol *Next(void) 		{ return next; }
	void Bind(VCD_symbol *nn) 	{ next = nn;   }
	~VCD_symbol();
};


typedef VCD_symbol *pvsymbol;

#endif /*VCD_SYMBOL_H_*/
