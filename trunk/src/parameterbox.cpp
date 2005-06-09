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
#include <qlabel.h>
#include <qlocale.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qpushbutton.h>

#include "parameterbox.h"
#include "triggerwidget.h"
#include "settings.h"

// -------------------------------------------------------------------------------------------------
ParameterBox::ParameterBox(QWidget* parent, const char* name)
    throw () 
    : QFrame(parent, name), m_leftValue(-1.0), m_rightValue(-1.0)
{
    QGridLayout* layout = new QGridLayout(this, 4 /* row */, 9 /* col */, 0 /* margin */, 5);
    
    // - create ------------------------------------------------------------------------------------
    
    // settings
    Settings& set = Settings::set();
    
    // widgets
    QLabel* timeLabel = new QLabel(tr("&Measuring Time:"), this);
    QLabel* triggerLabel = new QLabel(tr("&Triggering:"), this);
    
    QTimeEdit* timeedit = new QTimeEdit(this);
    timeedit->setRange(QTime(0, 0), QTime(0, 1));
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
    font.setPixelSize(15);
    font.setBold(true);
    m_leftMarker->setFont(font);
    m_rightMarker->setFont(font);
    m_diff->setFont(font);
    
    // buddys
    timeLabel->setBuddy(timeedit);
    triggerLabel->setBuddy(triggering);
    
    // load value
    triggering->setValue(set.readNumEntry("Measuring/Triggering/Value"),
                         set.readNumEntry("Measuring/Triggering/Mask"));
    timeedit->setTime(QTime(0, set.readNumEntry("Measuring/Triggering/Minutes"),
                               set.readNumEntry("Measuring/Triggering/Seconds"))); 
    
    // - layout the stuff --------------------------------------------------------------------------
                                   // row, col
    layout->addWidget(timeLabel,           0,    1);
    layout->addWidget(triggerLabel,        1,    1,    Qt::AlignTop);
    layout->addWidget(timeedit,            0,    3);
    layout->addMultiCellWidget(triggering, 1, 3, 3, 3);
    layout->addWidget(leftMarkerLabel,     0,    5);
    layout->addWidget(rightMarkerLabel,    1,    5);
    layout->addWidget(diffLabel,           2,    5);
    layout->addWidget(m_leftMarker,        0,    7,    Qt::AlignRight);
    layout->addWidget(m_rightMarker,       1,    7,    Qt::AlignRight);
    layout->addWidget(m_diff,              2,    7,    Qt::AlignRight);
    layout->setColStretch(0, 2);
    layout->setColSpacing(2, 20);
    layout->setColStretch(4, 4);
    layout->setColSpacing(6, 20);
    layout->setColSpacing(7, 130);
    layout->setColStretch(8, 2);
    
    connect(timeedit, SIGNAL(valueChanged(const QTime&)), SLOT(timeValueChanged(const QTime&)));
    connect(triggering, SIGNAL(valueChanged(byte, byte)), SLOT(triggerValueChanged(byte, byte)));
    
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
void ParameterBox::triggerValueChanged(byte mask, byte value)
    throw ()
{
    Settings& set = Settings::set();
    set.writeEntry("Measuring/Triggering/Value", value);
    set.writeEntry("Measuring/Triggering/Mask", mask);
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


// -------------------------------------------------------------------------------------------------
void ParameterBox::updateValues() throw ()
{
    QLocale loc;
    
    // update the left value
    if (m_leftValue < 0.0)
    {
        m_leftMarker->setText(tr("(no marker)"));
    }
    else
    {
        m_leftMarker->setText(loc.toString(m_leftValue, 'f', 4) + " ms");
    }
    
    
    // update the right value
    if (m_rightValue < 0.0)
    {
        m_rightMarker->setText(tr("(no marker)"));
    }
    else
    {
        m_rightMarker->setText(loc.toString(m_rightValue, 'f', 4) + " ms");
    }
    
    // update the diff display
    if (m_leftValue < 0.0 || m_rightValue < 0.0)
    {
        m_diff->setText(tr("(no difference)"));
    }
    else
    {
        m_diff->setText(loc.toString(m_rightValue - m_leftValue, 'f', 4) + " ms");
    }
}
