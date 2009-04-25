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
#include <QLayout>
#include <Q3VBox>
#include <Q3VBoxLayout>
#include <QLabel>

#include "triggerwidget.h"

// -------------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------------
TriggerWidget::TriggerWidget(QWidget* parent, const char* name)
    throw ()
    : QWidget(parent, name)
{
    Q3VBoxLayout* boxLayout = new Q3VBoxLayout(this, 0, 0);

    Q3HBox* hbox = new Q3HBox(this);
    hbox->setSpacing(2);

    // create the checkboxes with their labels
    for (int i = 0; i < NUMBER_OF_BITS_PER_BYTE; i++)
    {
        Q3VBox* box = new Q3VBox(hbox);
        box->setSpacing(2);

        m_labels[i] = new QLabel(QString::number(i+1), box);
        m_labels[i]->setAlignment(Qt::AlignHCenter);

        m_checkboxes[i] = new QCheckBox(box);
        m_checkboxes[i]->setTristate(true);
        m_checkboxes[i]->setNoChange();


        connect(m_checkboxes[i], SIGNAL(stateChanged(int)), SLOT(valueChangedHandler()));
    }

    boxLayout->addWidget(hbox);
    boxLayout->addStretch(5);
}


// -------------------------------------------------------------------------------------------------
byte TriggerWidget::getMask() const
    throw ()
{
    byte ret = 0;

    for (int i = 0; i < NUMBER_OF_BITS_PER_BYTE; i++)
    {
        ret |= (m_checkboxes[i]->checkState() != Qt::PartiallyChecked) << i;
    }

    PRINT_TRACE("getMask, return %x", ret);
    return ret;
}


// -------------------------------------------------------------------------------------------------
byte TriggerWidget::getValue() const
    throw ()
{
    byte ret = 0;

    for (int i = 0; i < NUMBER_OF_BITS_PER_BYTE; i++)
    {
        ret |= (m_checkboxes[i]->checkState() == Qt::Checked) << i;
    }

    PRINT_TRACE("getValue, return %x", ret);
    return ret;
}


// -------------------------------------------------------------------------------------------------
void TriggerWidget::valueChangedHandler()
    throw ()
{
    emit valueChanged(getMask(), getValue());
}


// -------------------------------------------------------------------------------------------------
void TriggerWidget::setValue(byte value, byte mask)
    throw ()
{
    for (int i = 0; i < NUMBER_OF_BITS_PER_BYTE; i++)
    {
        if (!(mask & (1 << i)))
        {
            m_checkboxes[i]->setNoChange();
        }
        else
        {
            m_checkboxes[i]->setChecked(value & (1<< i));
        }
    }
}

// vim: set sw=4 ts=4 tw=100:
