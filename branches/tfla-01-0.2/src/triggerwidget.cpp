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
#include <qlayout.h>
#include <qvbox.h>

#include "triggerwidget.h"

// -------------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------------
TriggerWidget::TriggerWidget(QWidget* parent, const char* name)
    throw ()
    : QWidget(parent, name)
{
    QVBoxLayout* boxLayout = new QVBoxLayout(this, 0, 0);
    
    QHBox* hbox = new QHBox(this);
    hbox->setSpacing(2);
    
    // create the checkboxes with their labels
    for (int i = 0; i < NUMBER_OF_WIRE_PER_SAPMPLE; i++)
    {
        QVBox* box = new QVBox(hbox);
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
uint TriggerWidget::getMask() const
    throw ()
{
    uint ret = 0;
    
    for (int i = 0; i < NUMBER_OF_WIRE_PER_SAPMPLE; i++)
    {
        ret |= (m_checkboxes[i]->state() != QButton::NoChange) << i;
    }
    
    PRINT_TRACE("getMask, return %x", ret);
    return ret;
}


// -------------------------------------------------------------------------------------------------
uint TriggerWidget::getValue() const
    throw ()
{
    uint ret = 0;
    
    for (int i = 0; i < NUMBER_OF_WIRE_PER_SAPMPLE; i++)
    {
        ret |= (m_checkboxes[i]->state() == QButton::On) << i;
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
void TriggerWidget::setValue(uint value, uint mask)
    throw ()
{
    for (int i = 0; i < NUMBER_OF_WIRE_PER_SAPMPLE; i++)
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

