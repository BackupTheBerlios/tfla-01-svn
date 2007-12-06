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
 * Class for formatin and sending to output sampled data
 */
  
#include "io_vcl.h"
#include <math.h>
#include <qlocale.h>

io_vcl::io_vcl()
{
	ss = NULL;
	time_postiton=0;
	time_exponent=-9; //1E-9
}

io_vcl::~io_vcl()
{
}

static void VcdTimeToStr(QString &ss, double tm)
{
	QLocale loc;
	double bs;
	bs = fabs(tm);
	int np;
	if (bs >= 1.0)
	{
		ss = loc.toString(floor(bs+0.5),'f',0)+" s";
		return;
	}
	np=0;
	while (bs < 1.0)
	{
		bs *=10;
		np -=1;
	}

	switch (np % 3)
	{
		case -1:
			bs *=100;
			np -=2;
		break;
		case -2:
			bs *=10;
			np -=1;
		break;
		
		default:
		break;
	}
	ss=loc.toString(bs,'f',0)+" ";
	switch (np)
	{
		case 0:
			ss += "S";
		break;

		case -3:
			ss += "ms";
		break;

		case -6:
			ss += "us";
		break;

		case -9:
			ss += "ns";
		break;

		case -12:
			ss += "ps";
		break;

		case -15:
			ss += "fs";
		break;

		default:
			ss += " 1E";
			ss += np;
		break;
	}
}

int io_vcl::Open(QFile &file, int trans_mode, QDateTime &dt)
{

	unsigned kk;
	QString s;
	QString sp;
	ss = new QTextStream(&file);
	x_time = 100E-9;
	magnum_time = measure_time/x_time; /* Mikrosekundy */
	magnum_time /= measure_samples;
	
	// first Sample must be >0
	// expresion for trunc is TIME=floor(0.5+magnum_time*nr_sample)
	VcdTimeToStr(s,x_time);
	switch (trans_mode)
	{
		case 0:
			sp = ";";
			mode =trans_mode;
			*ss << "_time_ " + s + sp;
			for (int i=0; i<num_wires; i+=1)
			{
				*ss << " D";
				*ss << i;
				if (i<num_wires-1)
				{
					*ss <<	sp;
				}
			}
			*ss << "\n";
		break;
		
		case 1:
			sp = ";";
			mode =trans_mode;
			magnum_time = 1;
			x_time  = measure_time;
			x_time /= measure_samples;  
			VcdTimeToStr(s,x_time);

			*ss << "_step_ T=" + s + sp;
			for (int i=0; i<num_wires; i+=1)
			{
				*ss << " D";
				*ss << i;
				if (i<num_wires-1)
				{
					*ss <<	sp;
				}
			}
			*ss << "\n";
		break;

	case 2:
		kk= mode = 2;
		*ss << "$date\n";
		*ss << dt.toString("yyyy-mm-dd hh:mm:ss \n");
		*ss << "$end\n";
		// for a while hard-coded string
		*ss << "$version Fabolous Logic Analyzer 1.0  $end\n";

		*ss << "$timescale ";
		*ss << s + " $end\n";

		*ss << "$scope module TFLA $end\n";
		for (int i=0; i<num_wires; i+=1)
		{
			*ss << "$var wire 1 ";
			*ss << (char)(i+'!');
			*ss << " D";
			*ss << i;
			*ss << " $end\n";
		}
		*ss << "$upscope $end\n";

		*ss << "$enddefinitions $end\n";
		break;

	default:
		mode =-1;
		//			delete(ss);
		break;
	}
	return (0);
}

void io_vcl::SendCVS_line(unsigned char sep)
{
	char bf[300];
	int n;
	double spm;

	if (mode != 1)
	{
		spm = floor(0.5+magnum_time*nr_sample);
	}
	else
	{
		spm = nr_sample;
	}

	n=sprintf(bf, "%d%c", int(spm), sep);
	for (int j = 0; j < num_wires; ++j)
	{
		if (current_sample & (1<<j))
		{
			bf[n]='1';
		} else
		{
			bf[n]='0';
		}
		bf[n+1]=sep;
		n+=2;
	}
	bf[n-1]='\n'; /* end of records have not separator*/
	bf[n++]=0;
	*ss << bf;
}

void io_vcl::SendVCD_line(unsigned new_sample)
{
	char bf[500];
	int n;
	double spm;
	unsigned n_mask;
	spm = floor(0.5+magnum_time*nr_sample);
	n=sprintf(bf, "#%d\n", int(spm));
	if (current_sample == new_sample)
	{
		n_mask = 0xFFFFFFFF;
	} else
	{
		n_mask = current_sample ^ new_sample;
	}
	for (int j = 0; j < num_wires; ++j)
	{
		if (n_mask & (1<<j))
		{
			if (j>0)
			{
				bf[n++]=' ';
			}
			if (new_sample & (1<<j))
			{
				bf[n++]='1';
			} else
			{
				bf[n++]='0';
			}
			bf[n++]='!'+j;
		}
	}
	bf[n++]='\n';
	bf[n++]=0;
	*ss << bf;
}

void io_vcl::SendFirstSample(unsigned sample)
{
	current_sample = sample;
	switch (mode)
	{
	case 0:
	case 1:
		nr_sample = 0;
		SendCVS_line(';');
		break;

	case 2:
		nr_sample = 0;
		*ss << "$dumpvars\n";
		SendVCD_line(current_sample);
		*ss << "$end\n";
		break;

	}
}

void io_vcl::SendSample(unsigned sample)
{
	nr_sample += 1;
	switch (mode)
	{
		case 0:
		case 1:
			if (sample != current_sample)
			{
				current_sample = sample;
				SendCVS_line(';');
			}
		break;

		case 2:
			if (sample != current_sample)
			{
				SendVCD_line(sample);
				current_sample = sample;
			}
		break;
	}
}

void io_vcl::SendLastSample(unsigned sample)
{
	current_sample = sample;
	switch (mode)
	{
		case 0:
		case 1:
			current_sample = sample;
			SendCVS_line(';');
		break;
	
	case 2:
//		if (sample != current_sample)
//		{
			SendVCD_line(sample);
			current_sample = sample;
//		}
		break;
	}
}

void io_vcl::SetTimeScale(unsigned n_samples, double s_time)
{
	measure_time = s_time;
	measure_samples = n_samples;
}

void io_vcl::Close(void)
{
	if (ss)
	{
		delete(ss);
		ss= NULL;
	}
	mode = -1;
}
