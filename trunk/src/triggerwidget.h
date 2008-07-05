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
#ifndef TRIGGERWIDGET_H
#define TRIGGERWIDGET_H

#include <qwidget.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qdatetime.h>

#include "global.h"

class TriggerWidget : public QWidget
{
     Q_OBJECT

     public:
        TriggerWidget(QWidget* parent, const char* name = 0) throw ();
        byte getMask() const throw ();
        byte getValue() const throw ();
        void setValue(byte mask, byte value) throw ();

    protected slots:
        void valueChangedHandler() throw ();

    signals:
        void valueChanged(byte mask, byte value);

    private:
        QLabel*         m_labels[NUMBER_OF_BITS_PER_BYTE];
        QCheckBox*      m_checkboxes[NUMBER_OF_BITS_PER_BYTE];
};

#endif /* TRIGGERWIDGET_H */

// vim: set sw=4 ts=4 tw=100:
