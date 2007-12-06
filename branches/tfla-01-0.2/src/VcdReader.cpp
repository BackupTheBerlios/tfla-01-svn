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

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "VcdReader.h"

enum Tokens   { T_VAR, T_END, T_SCOPE, T_UPSCOPE,
		T_COMMENT, T_DATE, T_DUMPALL, T_DUMPOFF, T_DUMPON,
		T_DUMPVARS, T_ENDDEFINITIONS, 
		T_DUMPPORTS, T_DUMPPORTSOFF, T_DUMPPORTSON, T_DUMPPORTSALL,
		T_TIMESCALE, T_VERSION, /* 17 elementow */
		T_EOL, T_STRING, T_UNKNOWN_KEY };

char *tokens[]={ "var", "end", "scope", "upscope",
		 "comment", "date", "dumpall", "dumpoff", "dumpon",
		 "dumpvars", "enddefinitions",
		 "dumpports", "dumpportsoff", "dumpportson", "dumpportsall",
		 "timescale", "version",
		 "", "", "" };

#define NUM_TOKENS 17

enum VarTypes { V_EVENT, V_PARAMETER,
                V_INTEGER, V_REAL, V_REAL_PARAMETER=V_REAL, V_REG, V_SUPPLY0,
                V_SUPPLY1, V_TIME, V_TRI, V_TRIAND, V_TRIOR,
                V_TRIREG, V_TRI0, V_TRI1, V_WAND, V_WIRE, V_WOR, V_PORT, V_IN=V_PORT, V_OUT=V_PORT, V_INOUT=V_PORT,
                V_END, V_LB, V_COLON, V_RB, V_STRING };



static char *vartypes[]=
{ 
	"event", 	"parameter",	"integer", 	"real", 
    "real_parameter", "reg", 	"supply0",	"supply1", 
    "time", 	"tri", 			"triand", 	"trior",
    "trireg", 	"tri0", 		"tri1", 	"wand", 
    "wire", 	"wor", 			"port", 	"in", 
    "out",     	"inout",		"$end", 	"", 
    "", 		"", 			""
};

#define NUM_VTOKENS 23 


static const unsigned char varenums[] = 
{  
	V_EVENT, V_PARAMETER,	V_INTEGER, V_REAL, 
    V_REAL_PARAMETER, 		V_REG, V_SUPPLY0,
    V_SUPPLY1, V_TIME, V_TRI, V_TRIAND, V_TRIOR,
    V_TRIREG, V_TRI0, V_TRI1, V_WAND, V_WIRE, V_WOR, V_PORT, V_IN, V_OUT, V_INOUT,
    V_END, V_LB, V_COLON, V_RB, V_STRING 
};




static void evcd_strcpy(char *dst, char *src)
{
	static char *evcd="01DUNZduLHXTlh01?FAaBbCcf";
	static char  *vcd="0101xz0101xz0101xzxxxxxxx";

	char ch;
	int i;

	while((ch=*src))
	{
		for(i=0;i<25;i++)
		{
			if(evcd[i]==ch)
			{
				*dst=vcd[i];
				break;
			}
		}	
		if(i==25) *dst='x';

		src++;
		dst++;
	}

	*dst=0;	/* null terminate destination */
}


VcdReader::VcdReader()
{
	format=0;
	sym_root=NULL;
	memset(sec_version,0,sizeof(sec_version));
	memset(sec_date,0,sizeof(sec_date));
	memset(sec_device,0,sizeof(sec_device));
	s_pos = s_max=0;
	phase = S_Init; 
	wire_num =0;
	arg_num =0;
	unit_factor = 1E-9;
	unit_step 	= 1;
	vcd_time  = 0;
	vcd_param = 0;
	tok_id = T_EOL;
}


int VcdReader::split_csv_to_var(char sep)
{
	int i,k;
	if (s_pos==s_max)
	{
		return(0);
	}
	k=0;
	for (i=0; i<MAXWIRE+1 && s_line[k] !=0 ; i+=1)
	{
		var_s[i]=&s_line[k];
		var_l[i]=0;
		
		while (s_line[k] != sep && k < (int)s_max)
		{
			k+=1;
			var_l[i]+=1;
		}
		if (s_line[k]!=sep)
		{
			break;
		}
		k+=1;
	}
	return(i+1);
}

pvsymbol VcdReader::search_symbol(char *sym_id)
{
	int k;
	pvsymbol fi;
	k = 0;
	for (fi=sym_root; fi != NULL; fi = fi->Next())
	{
		k+=1;
		if (k>1000)
		{
			return(NULL);
		}
		if (strcmp(sym_id,fi->Id())==0)
		{
			return(fi);
		}
	} 
	return(fi);
}

pvsymbol VcdReader::add_symbol(char *sym_id)
{
	pvsymbol fi =  search_symbol(sym_id);
	if (fi != NULL)
	{
		return(fi);
	}
	fi = new VCD_symbol;
	if (fi == NULL)
	{
		return(NULL);
	}
	fi->SetId(sym_id);
	fi->Bind(sym_root);
	sym_root = fi;		
	return(fi);
}

int VcdReader::GetRawToken(const char *src)
{
	unsigned k;
	char cd;
	tok_len = 0;
	k =0;
	while (isspace(src[k]) || src[k] == '\n')
	{
		 k+=1;
	}
	if (src[k]==0)
	{
		tok_bf[0]=0;
		return(k);
	}

	do
	{
		cd = src[k++];
		if (tok_len < sizeof(tok_bf)-1)
		{
			tok_bf[tok_len++]=cd;
		}
		cd = src[k];
		if ((isspace(src[k]) || cd == '\n'))
		{
			break;
		}
	} 	while (cd != 0 ) ;
	tok_bf[tok_len]=0;
	return(k);
}

int VcdReader::GetToken(void)
{
	unsigned k;
	tok_len = 0;
	tok_id  = -1;
	
	if (s_pos == s_max || s_line[s_pos]=='\n')
	{
		s_pos=s_max;
		tok_id = T_EOL;
		return(tok_id);
	}
	k=GetRawToken(&s_line[s_pos]);
	s_pos += k;

	switch (tok_bf[0])
	{
		case '$':
			for (k=0; k != T_EOL; k+=1)
			{
				if (strcmp(&tok_bf[1],tokens[k])==0)
				{
					tok_id = k;
					break;
				}
			}
			if (tok_id == T_EOL)
			{
				tok_id = T_UNKNOWN_KEY;
			}
		break;

		case '\n':
			tok_id = T_EOL;
		break;

		case '#':
			tok_id = T_STRING;
		break;		

		default:
			tok_id = T_STRING;
		break;
	}
	return(tok_id);
}


int VcdReader::SetMode(int mcode)
{
	switch (mcode)
	{
		case 1:
			format =1;
			phase = S_Init;
		break;

		case 2:
			format =2;
			phase = S_Init;
		break;

		default:
		return(1);
	}
	return(0);
}

int VcdReader :: HeaderCSV(void)
{
	int num_field;

  	if (s_line[0]=='#')
  	{
		s_pos = s_max;
	  	return(VCD_RC_COMMENT); /* komentarz */
  	}		
	if (s_max < 6 || strncmp( "_time_",s_line,6)!=0)
	{
			s_pos = s_max;
			return(VCD_RC_NOP);
	}
	s_pos =0;
	c_sep =';';
	num_field =split_csv_to_var(c_sep);
	s_pos = s_max;
	
	if (num_field > 1)
	{
		phase=S_DataStart;
		unit_factor = 1E-9;
		unit_step   = 100;
		wire_num = num_field-1;
		return(VCD_RC_DATA_START);
	}
	s_pos = s_max;
	phase=S_End;
	return(VCD_RC_EOF);

}

int VcdReader :: SampleCSV(void)
{
	char bf[80];
	unsigned i;
	int k;
	long f_time;

  	if (split_csv_to_var(c_sep)!=wire_num+1)
  	{
		s_pos = s_max;
	  	return(VCD_RC_NO_DATA);
  	}

	i = (var_l[0] < 16)? var_l[0]:16;
	memcpy(bf,var_s[0],i);
	bf[i]=0;

	s_pos = s_max;
				
				
	if (sscanf(bf,"%ld",&f_time)==1)
	{
		for (k=1; k<wire_num; k+=1)
		{
			i = (var_l[k] < 16)? var_l[k]:16;
			memcpy(bf,var_s[k],i);
			bf[i]=0;
			if (sscanf(bf,"%ld",&c_val[k-1])!=1)
			{
				c_val[k-1]=0;
			}
		}
		c_time = f_time;
		if (phase == S_DataStart)
		{
			phase = S_Data;	
		}
		return(VCD_RC_DATA);			
	}
	
	phase = S_End;

	return(VCD_RC_EOF); // 2oprintf( outfile, csv_line );
}


int VcdReader :: CopyToken(char *dst,unsigned mx)
{
	unsigned i,k;
	k = strlen(dst);
	for (i=0; i<tok_len; i+=1)
	{
		if (k+1 < mx)
		{
			dst[k++]=tok_bf[i];			
		}
		else
		{
			break;
		}
	}
	dst[k]=0;
	return(i);
}

void VcdReader :: TokenToArg(void)
{
	if (arg_num == 0 )
	{
		memcpy(arg_page,tok_bf,tok_len);
		arg_num=tok_len;
		arg_page[arg_num]=0;
	}
	else
	{
		if (arg_num + tok_len < sizeof(arg_page)-6)
		{
			arg_page[arg_num++]=' ';
			memcpy(&arg_page[arg_num],tok_bf,tok_len);
			arg_num += tok_len;
			arg_page[arg_num]=0;
		}
	}
}

void VcdReader ::ScanVarSection(void)
{
	unsigned pos;
	int   var_type;
	int   sx;
	pvsymbol  ss;
//	char  *name;
	pos = GetRawToken(arg_page);
	for (var_type=0; var_type<NUM_VTOKENS; var_type+=1)
	{
		if (strcmp(&tok_bf[0],vartypes[var_type])==0)
		{
			break;
		}
	}
	if (var_type == NUM_VTOKENS)
	{
		return;
	}
	var_type = varenums[var_type]; //mapowanie
	if (arg_num == pos)
	{
		return;
	}
	pos += GetRawToken(&arg_page[pos]);
	if (arg_num == pos)
	{
		return;
	}
	if (sscanf(tok_bf,"%d",&sx) != 1)
	{
		sx  = 1;
		return;
	}
	pos += GetRawToken(&arg_page[pos]);
	if (arg_num == pos)
	{
		return;
	}
	ss=add_symbol(tok_bf);
	if (ss != NULL)
	{
		pos += GetRawToken(&arg_page[pos]);
		if (tok_len > 0)
		{
			ss->SetName(tok_bf);
			ss->vartype = var_type;
			if (var_type == V_WIRE && wire_num < MAXWIRE)
			{
				ss->size = 1;
				ss->wire_nr=wire_num++;
			}
			else
			{
				ss->size =sx;
				ss->wire_nr = -1;
			}
		}
	}
}

void VcdReader ::ScanTimeSection(void)
{
// Power, unit 	
	unsigned pos;
	unsigned mul;
	double   exp;
	
	pos = GetRawToken(arg_page);
	if (sscanf(tok_bf,"%d",&mul) != 1)
	{
		mul =1;
		
	}
	else
	{
		if (mul<1 || mul > 10000)
		{
			mul= 1;
		}
		 
	}
	if (arg_num == pos)
	{
		exp = 1E-9;
		return;
	}
	else
	{
		pos += GetRawToken(&arg_page[pos]);
		switch(tok_bf[0])
		{
			case 's': exp = 1; break;
			case 'm': exp = 1E-3; break;
			case 'u': exp = 1E-6; break;
			case 'n': exp = 1E-9; break;
			case 'p': exp = 1E-12; break;
			case 'f': exp = 1E-15; break;
			default: 
				exp = 1E-9; 
			break;
		}
	}
	unit_step = mul;
	unit_factor  = exp;
	while (unit_step >= 1000)
	{
		unit_step /= 10;
		unit_factor *=10;
	}
}

int VcdReader :: HeaderVCD(void)
{
	int k;
	switch (phase)
	{
		case S_Init:
			GetToken();
			switch(tok_id)
			{
				case T_DATE:
				case T_VERSION:
					vcd_section_id=tok_id;
					vcd_param=1;
					arg_page[0]=0;
					arg_num=0;
					phase =S_Header;
				break;				

				case T_EOL:
				return(VCD_RC_NO_DATA);
				
				default:
					vcd_section_id =T_UNKNOWN_KEY;
					vcd_param=0;
				break;
			}
			if (phase != S_Header)
			{
				break;
			}

		case S_Header:
			if (vcd_param == 0)
			{
				GetToken();
				switch(tok_id)
				{
					case T_DATE:
					case T_VERSION:
					case T_SCOPE:
					case T_UPSCOPE:
					case T_TIMESCALE:
					case T_VAR:
					case T_ENDDEFINITIONS:
						vcd_section_id=tok_id;
						vcd_param=1;
						arg_page[0]=0;
						arg_num=0;
					break;
					case T_EOL:
						s_pos = s_max;
					return(VCD_RC_NO_DATA);

					default:
						vcd_section_id =T_UNKNOWN_KEY;
						vcd_param=0;
						s_pos = s_max = 0;
						// ignorowanie linii z nieznanymi paramterami
					return(VCD_RC_NOP);
				}
				
			}
			if (vcd_param == 1)
			{
				do
				{
					if (GetToken()==T_EOL)
					{
						return(VCD_RC_NO_DATA);
					}
					if (tok_id != T_END)
					{
						TokenToArg();
					}
					else
					{
						vcd_param =2;
					}
				} while (vcd_param != 2 );
			}
			/* Koniec sekcji wykonac analize i skasowac dane */
			switch(vcd_section_id)
			{
				case T_DATE:
					k = (arg_num < sizeof(sec_date)-1) ? arg_num : sizeof(sec_date)-1;
					memcpy(sec_date,arg_page,k);
					sec_date[k]=0; 
				break;
										
				case T_VERSION:
					k = (arg_num < sizeof(sec_version)-1) ? arg_num : sizeof(sec_version)-1;
					memcpy(sec_version,arg_page,k);
					sec_date[k]=0; 
				break;

				case T_SCOPE:
					k = (arg_num < sizeof(sec_device)-1) ? arg_num : sizeof(sec_device)-1;
					memcpy(sec_device,arg_page,k);
					sec_date[k]=0; 
				break;
				
				case T_VAR:
					ScanVarSection();
				break;

				case T_TIMESCALE:
					ScanTimeSection();
				break;
				
				case T_UPSCOPE:
				break;
				

				case T_ENDDEFINITIONS:
					c_counter =0;
					phase =S_DataStart;
					tok_id = T_DUMPVARS;
				break;
				
				default:
				break;
			}
			vcd_param =0;
		break;

		case S_DataStart:
		case S_Data:
		return(SampleVCD());


		case S_End:
			s_pos = s_max;
		return(VCD_RC_NO_DATA);
	}
	if (s_pos==s_max-1 && s_line[s_pos]=='\n')
	{
		s_pos+=1;
	}
	if (s_pos<s_max-1 )
	{
		return(VCD_RC_NOP);
	}
	else
	{
		return(VCD_RC_NO_DATA);
	}
}

int VcdReader :: ScanTimeVar(void)
{
	int k;
	pvsymbol  ss;
	if (c_counter == 0)
	{
		vcd_section_id = T_END;
		sscanf(&tok_bf[1],"%ld",&vcd_time);
		c_counter+=1;
		return(VCD_RC_DATA_START);
	}
	// marker czasowy
	c_time = vcd_time;
	for (ss=sym_root; ss != NULL; ss = ss->Next())
	{
		k = ss->wire_nr;
		if (k>=0 && k< MAXWIRE)
		{
			c_val[k] = ss->GetValue();
		}
	}
	sscanf(&tok_bf[1],"%ld",&vcd_time);
	c_counter+=1;
	return(VCD_RC_DATA);

}
int VcdReader :: ScanDiffVar(void)
{
	unsigned k;
	pvsymbol  ss;
	char bf[40];
	// translacja wektora na jeszcze cos lub scalar
	// <liczba bitow > ' <podstatwa> <cyfry>
	// podstaw h,b,d,o
	k=0;
	if (vcd_param==0)
	{
		arg_num = 0;
	}
	TokenToArg();
	switch(arg_page[0])
	{
		case '0':
        case '1':
        case 'x': case 'X':
        case 'z': case 'Z':
        case 'h': case 'H':
        case 'u': case 'U':
        case 'w': case 'W':
        case 'l': case 'L':
        case '-':
			ss = search_symbol(&arg_page[1]);
			if (ss != NULL)
			{
				arg_page[1]=0;
				evcd_strcpy(bf,arg_page);
				ss->SetValue(bf);		
			}
			vcd_param = 0;
			arg_num = 0;
		break;
		
		case 'b':
		case 'B':
			// vektor binanrny + token
			if (vcd_param == 0)
			{
				vcd_param +=1;
				break;
			}
			ss = search_symbol(tok_bf);
			if (ss != NULL)
			{
				bf[0]='b';
				evcd_strcpy(&bf[1],&arg_page[1]);
				ss->SetValue(bf);		
			}
			vcd_param = 0;
			arg_num = 0;		
		break;
		
		case 'p':
			/* extract port dump value.. */
			vcd_param +=1;
			if (vcd_param < 3)
			{
				break;
			}
			vcd_param = 0;
			arg_num  =0;
		break;
		
		case 'r':
		case 'R':
			/* double */
			if (vcd_param == 0)
			{
				vcd_param +=1;
				break;
			}
			ss = search_symbol(tok_bf);
			if (ss != NULL)
			{
				arg_page[0]='r';
				ss->SetValue(arg_page);		
			}
			vcd_param = 0;
			arg_num = 0;		
		break;
		
		case 's':
		case 'S':
			if (vcd_param == 0)
			{
				vcd_param +=1;
				break;
			}
			vcd_param = 0;
			arg_num = 0;		
			//get_strtoken();	/* simply skip for now */
		break;
		
		default:
		break;
	}
	if (s_pos==s_max-1 && s_line[s_pos]=='\n')
	{
		s_pos+=1;
	}
	if (s_pos<s_max)
	{
		return(VCD_RC_NOP);
	}
	else
	{
		return(VCD_RC_NO_DATA);
	}

}

int VcdReader :: SampleVCD(void)
{
	switch (phase)
	{
		case S_Init:
		case S_Header:
		return(HeaderVCD());

		case S_DataStart:
			GetToken();
			if (s_pos==s_max-1 && s_line[s_pos]=='\n')
			{
				s_pos+=1;
			}
			switch(tok_id)
			{
				case T_DUMPVARS:
					phase =S_Data;
					vcd_section_id = tok_id;
				break;

				case T_STRING:
					if (tok_bf[0]=='#')
					{
						if (c_counter>0)
						{
							phase = S_Data;
						}
						vcd_param = 0;
						return(ScanTimeVar());
					}
				return(ScanDiffVar());
				
				case T_EOL:
				return(VCD_RC_NO_DATA);
				
				case T_END:
					vcd_section_id = T_END;
				return(VCD_RC_NOP);
			}
		break;
		
		case S_Data:
			GetToken();
			if (s_pos==s_max-1 && s_line[s_pos]=='\n')
			{
				s_pos+=1;
			}
			switch (tok_id)
			{
				case T_EOL:
				return(VCD_RC_NO_DATA);
			
				case T_END:
				return(VCD_RC_NOP);
				case T_STRING:
					if (tok_bf[0]=='#')
					{
						vcd_param = 0;
						return(ScanTimeVar());
					}
				return(ScanDiffVar());
			}
		break;

		case S_End:
			s_pos = s_max=0;
		return(VCD_RC_NO_DATA);
	}
	if (s_pos==s_max-1 && s_line[s_pos]=='\n')
	{
		s_pos+=1;
	}
	
	if (s_pos<s_max)
	{
		return(VCD_RC_NOP);
	}
	else
	{
		return(VCD_RC_NO_DATA);
	}
}


void VcdReader::InputLine(const char *ss)
{
	if (ss==NULL)
	{

		s_line="";
	}
	else
	{
		s_line = ss;
	}
	s_pos  = 0;
	s_max  = strlen(ss);
}
int  VcdReader::ScanData(void)
{ 
	if (s_max==0 || s_pos==s_max)
	{
		s_pos=s_max=0;
		return(VCD_RC_NO_DATA);
	}
	switch (phase)
	{
		case S_Init:
		case S_Header:
			switch (format)
			{
				case 1:	
				return(HeaderCSV());
				
				case 2: 
				return(HeaderVCD());
				
				default:
				break;
			}
		break;
		case S_DataStart:
		case S_Data:
			switch (format)
			{
				case 1:	
				return(SampleCSV());
				case 2: 
				return(SampleVCD());
				default:
				break;
			}
		break;
		case S_End:
			s_pos=s_max=0;
		return(VCD_RC_NOP);
	}
	return(0);
}

int  VcdReader::EndOfData(void)
{ 
	int k;
	pvsymbol  ss;
	if (phase == S_Data)
	{
		switch (format)
		{
			case 1:	
			break;
			
			case 2: 
				// marker czasowy
				c_time = vcd_time;
				for (ss=sym_root; ss != NULL; ss = ss->Next())
				{
					k = ss->wire_nr;
					if (k>=0 && k< MAXWIRE)
					{
						c_val[k] = ss->GetValue();
					}
				}
				phase = S_End;
			return(VCD_RC_DATA);
			
			default:
			break;
		}
	}
	s_pos=s_max=0;
	phase = S_End;
	return(VCD_RC_NOP);
}

double VcdReader::GetSampleTime(void)
{
	double	ss = unit_step*unit_factor;;
	return(ss);
}

VcdReader::~VcdReader()
{
	pvsymbol fi;
	while (sym_root != NULL)
	{
		fi = sym_root;
		sym_root = fi->Next();
		delete fi;
	}
}
