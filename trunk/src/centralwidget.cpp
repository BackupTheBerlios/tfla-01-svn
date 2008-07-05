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
#include <qgroupbox.h>
#include <qpushbutton.h>

#include "centralwidget.h"
#include "dataview.h"

// -------------------------------------------------------------------------------------------------
CentralWidget::CentralWidget(QWidget* parent, const char* name)
    throw ()
    : QFrame(parent, name)
{
    QVBoxLayout* layout = new QVBoxLayout(this, 6);
    m_dataView = new DataView(this);

    layout->addWidget(m_dataView);
    layout->setStretchFactor(m_dataView, 10);

    QGroupBox* groupBox = new QGroupBox(1, Horizontal, this);
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
