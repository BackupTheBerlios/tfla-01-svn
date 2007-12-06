#ifndef IO_VCL_H_
#define IO_VCL_H_

#include <qtextstream.h>
#include <qfile.h>
#include <qdatetime.h>

class io_vcl
{
protected:
	unsigned 	time_postiton;
	int			time_exponent;
	
	unsigned 	nr_sample;
	unsigned	current_sample;
	double		magnum_time; /* krok w nanosekundach */
	double		x_time;			 /* poadtsawa_czasu        */
	
	int			mode;
	QTextStream		*ss;
	double		measure_time;
	unsigned	measure_samples;
public:
	io_vcl();
	int		 num_wires;
	unsigned valid_wires;	// maska kana³ow w wyniku
	
	int   Open(QFile &file,int trans_mode,QDateTime &m_start);
	void  SendFirstSample(unsigned sample);
	void  SendSample(unsigned sample);
	void  SendLastSample(unsigned sample);
	void  SendCVS_line(unsigned char sep);
	void  SendVCD_line(unsigned new_sample);
	void  SetTimeScale(unsigned n_samples, double s_time); 
	void  Close(void);
	virtual ~io_vcl();
};

#endif /*IO_VCL_H_*/
