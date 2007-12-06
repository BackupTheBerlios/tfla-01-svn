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
#include <qapplication.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qslider.h>
#include <qlabel.h>
#include <qlocale.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qpushbutton.h>
#include <math.h>

#include "parameterbox.h"
#include "triggerwidget.h"
#include "settings.h"

#define MAX_SLIDER_VALUE 20

// -------------------------------------------------------------------------------------------------
ParameterBox::ParameterBox(QWidget* parent, const char* name)
    throw () 
    : QFrame(parent, name), m_leftValue(-1.0), m_rightValue(-1.0)
{
    QGridLayout* layout = new QGridLayout(this, 5 /* row */, 9 /* col */, 0 /* margin */, 5);
    
    // - create ------------------------------------------------------------------------------------
    
    // settings
    Settings& set = Settings::set();
    
    // widgets
    QLabel* timeLabel = new QLabel(tr("&Measuring Time:"), this);
    QLabel* sampleLabel = new QLabel(tr("&Sampling Rate:"), this);
    QLabel* triggerLabel = new QLabel(tr("&Triggering:"), this);
    
    QTimeEdit* timeedit = new QTimeEdit(this);
    timeedit->setRange(QTime(0, 0), QTime(0, 1));
    QSlider* sampleSlider = new QSlider(0, MAX_SLIDER_VALUE, 1, 0, Qt::Horizontal, this);
    TriggerWidget* triggering = new TriggerWidget(this);
    
    // labels for the markers
    QLabel* leftMarkerLabel = new QLabel(tr("Left Button Marker:"), this);
    QLabel* rightMarkerLabel = new QLabel(tr("Right Button Marker:"), this);
    QLabel* diffLabel = new QLabel(tr("Difference:"), this);
    m_leftMarker = new QLabel(this);
    m_rightMarker = new QLabel(this);
    m_diff = new QLabel("zdddd", this);
    
    // set the font for the labels
    QFont font = qApp->font(this);
	font.setPointSize(font.pointSize()*3/2);
    font.setBold(true);
    m_leftMarker->setFont(font);
    m_rightMarker->setFont(font);
    m_diff->setFont(font);
    
    // buddys
    timeLabel->setBuddy(timeedit);
    triggerLabel->setBuddy(triggering);
    sampleLabel->setBuddy(sampleSlider);
    
    // load value
    triggering->setValue(set.readNumEntry("Measuring/Triggering/Value"),
                         set.readNumEntry("Measuring/Triggering/Mask"));
    timeedit->setTime(QTime(0, set.readNumEntry("Measuring/Triggering/Minutes"),
                               set.readNumEntry("Measuring/Triggering/Seconds"))); 
    sampleSlider->setValue(MAX_SLIDER_VALUE - set.readNumEntry("Measuring/Number_Of_Skips"));
    
    // - layout the stuff --------------------------------------------------------------------------
                                   		//  row, col
    layout->addWidget(timeLabel,           	0,    1);
    layout->addWidget(sampleLabel,         	1,    1);
    layout->addWidget(triggerLabel,        	2,    1,    Qt::AlignTop);
    layout->addWidget(timeedit,            	0,    3);
	layout->addWidget(sampleSlider,        	1,    3);

	layout->addMultiCellWidget(triggering, 	2, 3, 3, 3);

	layout->addWidget(leftMarkerLabel,      0,    5);
    layout->addWidget(rightMarkerLabel,    	1,    5);
    layout->addWidget(diffLabel,         	2,    5);
    
	layout->addWidget(m_leftMarker,        	0,    7,    Qt::AlignRight);
    layout->addWidget(m_rightMarker,       	1,    7,    Qt::AlignRight);
    layout->addWidget(m_diff,           	2,    7,    Qt::AlignRight);

	layout->setColStretch(0, 2);
    layout->setColSpacing(2, 20);

	layout->setColStretch(4, 4);
    layout->setColSpacing(6, 20);
	layout->setColSpacing(7, 150);
    layout->setColStretch(8, 2);
    
    connect(timeedit,                   SIGNAL(valueChanged(const QTime&)), 
            this,                       SLOT(timeValueChanged(const QTime&)));
    connect(triggering,                 SIGNAL(valueChanged(unsigned, unsigned)), 
            this,                       SLOT(triggerValueChanged(unsigned, unsigned)));
    connect(sampleSlider,               SIGNAL(valueChanged(int)),
            this,                       SLOT(sliderValueChanged(int)));
    
    // - initial values ----------------------------------------------------------------------------
    updateValues();
}


// -------------------------------------------------------------------------------------------------
void ParameterBox::timeValueChanged(const QTime& time)
    throw ()
{
    Settings& set = Settings::set();
    set.writeEntry("Measuring/Triggering/Minutes", time.minute());
    set.writeEntry("Measuring/Triggering/Seconds", time.second());
}


// -------------------------------------------------------------------------------------------------
void ParameterBox::triggerValueChanged(unsigned mask, unsigned value)
    throw ()
{
    Settings& set = Settings::set();
    set.writeEntry("Measuring/Triggering/Value", (int)value);
    set.writeEntry("Measuring/Triggering/Mask", (int)mask);
}


// -------------------------------------------------------------------------------------------------
void ParameterBox::sliderValueChanged(int value) throw ()
{
    Settings::set().writeEntry("Measuring/Number_Of_Skips", MAX_SLIDER_VALUE - value);
}


// -------------------------------------------------------------------------------------------------
void ParameterBox::setLeftMarker(double ms) throw ()
{
    m_leftValue = ms;
    updateValues();
}


// -------------------------------------------------------------------------------------------------
void ParameterBox::setRightMarker(double ms) throw ()
{
    m_rightValue = ms;
    updateValues();
}

static void TimeToStr(QString &ss,double tm)
{
	QLocale loc;
	double bs;
	bs = fabs(tm);
	
	if (bs ==0)
	{
		ss="0.0000 ms";
		return;
	}
	int 
	kk=(int)log10(bs/1000);
	kk = 2 - (kk%3);

	
	if (bs >= 1.0)
	{
	
		if (bs > 2000)
		{
			ss=loc.toString(tm/1000, 'f', 3) + " s";
		}
		else
		{
			ss=loc.toString(tm, 'f', kk) + " ms";
		}
	}
	else if (bs >= 1E-3)
	{
		ss=loc.toString(tm*1E3, 'f', kk) + " us";
	}
	else if (bs >= 1E-6)
	{
		ss=loc.toString(tm*1E6, 'f', kk) + " ns";
	}
	else
	{
		ss=loc.toString(tm*1E9, 'f', kk) + " ps";
	}
}

// -------------------------------------------------------------------------------------------------
void ParameterBox::updateValues() throw ()
{
    QLocale loc;
    QString tm;
    // update the left value
    if (m_leftValue < 0.0)
    {
        m_leftMarker->setText(tr("(no marker)"));
    }
    else
    {
    	TimeToStr(tm,m_leftValue);
        m_leftMarker->setText(tm);
    }
    
    
    // update the right value
    if (m_rightValue < 0.0)
    {
        m_rightMarker->setText(tr("(no marker)"));
    }
    else
    {
    	TimeToStr(tm,m_rightValue);
        m_rightMarker->setText(tm);
    }
    
    // update the diff display
    if (m_leftValue < 0.0 || m_rightValue < 0.0)
    {
        m_diff->setText(tr("(no difference)"));
    }
    else
    {
    	TimeToStr(tm,m_rightValue - m_leftValue);
        m_diff->setText(tm);
    }
}
