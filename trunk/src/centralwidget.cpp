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
#include <qwidget.h>
#include <qlayout.h>
#include <q3groupbox.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <Q3Frame>
#include <Q3VBoxLayout>

#include "centralwidget.h"
#include "dataview.h"

// -------------------------------------------------------------------------------------------------
CentralWidget::CentralWidget(QWidget* parent, const char* name)
    throw ()
    : Q3Frame(parent, name)
{
    Q3VBoxLayout* layout = new Q3VBoxLayout(this, 6);
    m_dataView = new DataView(this);

    layout->addWidget(m_dataView);
    layout->setStretchFactor(m_dataView, 10);

    Q3GroupBox* groupBox = new Q3GroupBox(1, Qt::Horizontal, this);
    m_parametersBox = new ParameterBox(groupBox);

    layout->addWidget(groupBox);
    layout->setStretchFactor(groupBox, 0);

    connect(m_dataView,                SIGNAL(leftMarkerValueChanged(double)),
            m_parametersBox,           SLOT(setLeftMarker(double)));
    connect(m_dataView,                SIGNAL(rightMarkerValueChanged(double)),
            m_parametersBox,           SLOT(setRightMarker(double)));

}


// -------------------------------------------------------------------------------------------------
DataView* CentralWidget::getDataView() const
    throw ()
{
    return m_dataView;
}

// vim: set sw=4 ts=4 tw=100:
