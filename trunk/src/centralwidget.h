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
#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <qwidget.h>

#include "dataview.h"
#include "parameterbox.h"

class CentralWidget : public QFrame
{
    Q_OBJECT

    public:
        CentralWidget(QWidget* parent, const char* name = 0) throw ();

        DataView* getDataView() const throw ();
        ParameterBox* getParametersBox() const throw ();

    private:
        DataView*           m_dataView;
        ParameterBox*       m_parametersBox;
};


#endif /* CENTRALWIDGET_H */

// vim: set sw=4 ts=4 tw=100:
