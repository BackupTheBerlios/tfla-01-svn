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
 * Class for intepreting VCD or CSV stream
 */

#ifndef VCDREADER_H_
#define VCDREADER_H_

#include "VCD_symbol.h"


enum Vcdr_state
{
	S_Init=0,
	S_Header=1,
	S_DataStart=2,
	S_Data=3,
	S_End=9
};

#ifndef MAXWIRE
#define MAXWIRE 32
#endif

#define VCD_RC_NOP 			0
#define VCD_RC_DATA_START 	1
#define VCD_RC_DATA       	2
#define VCD_RC_DATA_LAST	3
#define VCD_RC_EOF		  	4
#define VCD_RC_COMMENT	  	5
#define VCD_RC_NO_DATA		6
#define VCD_RC_ERROR	  	9

#define TOKEN_SIZE  128

#define VCD_SEC_PAGE 4096


class VcdReader
{
//	char	line[512];
//	int 	( *src_gets )( char *, int sx);
public:
	char	sec_date[256];		/* data and time tokesn */
	char	sec_device[256];    /* measuring device name */
	char	sec_version[256];   /* measuring program version */
protected:
	const char	*s_line; // input line
	unsigned s_pos;		 // current char posiotion
	unsigned s_max;		 // input line lemgth
	char  	 c_sep;		 // field se[arator	
	
	char     arg_page[VCD_SEC_PAGE]; /* expresion completationbuffer */
	unsigned arg_num;   // number ow token in arg_page
	
	char	 tok_bf[TOKEN_SIZE];
	unsigned tok_len;
	int		 tok_id;

	long 	 vcd_time;	
	int		 vcd_param;
	int		 vcd_section_id;
	
	Vcdr_state 	phase;
	int 		format;
	pvsymbol 	sym_root;

	const char *var_s[MAXWIRE+1];
	int   		var_l[MAXWIRE+1];
	
	double	 unit_factor;
	int  	 unit_step;	/* unit = unit_stem * E^ unit_base; */
	
	
	int split_csv_to_var(char sep);
	pvsymbol add_symbol(char *sym_id);
	pvsymbol search_symbol(char *sym_id);
	
	void ScanVarSection(void);
	void ScanTimeSection(void);
	void ScanDumpSection(void);
	int  ScanDiffVar(void);  /* nowa wartosci wysylana */
	int  ScanTimeVar(void);  /* Is Time marker   */
	
	int	 GetRawToken(const char *src); 
	// copy token to buffer, returm token length
	
	int  CopyToken(char *new_dst, unsigned mx);
	void TokenToArg(void);
	
	int  GetToken(void);
	int  HeaderCSV(void);
	int  SampleCSV(void);
	int  HeaderVCD(void);
	int  SampleVCD(void);
	
public:
	long	c_counter;
	long 	c_time;
	int	 	wire_num;
	long  	c_val[MAXWIRE];

	VcdReader();
	double GetSampleTime(void);
	int SetMode(int mcode); /* 9=vcd 1=cvs */
	void InputLine(const char *ss);
	int ScanData(void);
	int EndOfData(void);
	
	Vcdr_state GetStatus(void) { return phase; }
	virtual ~VcdReader();
};

#endif /*VCDREADER_H_*/
