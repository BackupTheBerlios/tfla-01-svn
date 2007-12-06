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
#ifndef PARAMETERBOX_H
#define PARAMETERBOX_H

#include <qframe.h>
#include <qlabel.h>

#include "global.h"

class ParameterBox : public QFrame
{
    Q_OBJECT
    
    public:
        ParameterBox(QWidget* parent, const char* name = 0) throw ();
        virtual ~ParameterBox() {}
        
    public slots:
        void setLeftMarker(double value) throw ();
        void setRightMarker(double value) throw ();
        
    protected slots:
        void timeValueChanged(const QTime& time) throw (); 
        void triggerValueChanged(unsigned mask, unsigned value) throw ();
        void sliderValueChanged(int newValue) throw ();
        void updateValues() throw ();
        
    private:
        QLabel* m_leftMarker;
        QLabel* m_rightMarker;
        QLabel* m_diff;
        double  m_leftValue;
        double  m_rightValue;
};

#endif /* PARAMETERBOX_H */
