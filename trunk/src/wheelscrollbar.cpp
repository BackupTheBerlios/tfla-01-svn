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
#include <QWheelEvent>

#include "wheelscrollbar.h"

// -------------------------------------------------------------------------------------------------
WheelScrollBar::WheelScrollBar(QWidget * parent, const char* name)
    : QScrollBar(parent, name)
{}


// -------------------------------------------------------------------------------------------------
WheelScrollBar::WheelScrollBar(Qt::Orientation orientation, QWidget* parent, const char* name)
    : QScrollBar(orientation, parent, name)
{}


// -------------------------------------------------------------------------------------------------
WheelScrollBar::WheelScrollBar(int minValue, int maxValue, int lineStep, int pageStep,
                               int value,  Qt::Orientation orientation, QWidget* parent,
                               const char* name)
    : QScrollBar(minValue, maxValue, lineStep, pageStep, value, orientation, parent, name)
{}


// -------------------------------------------------------------------------------------------------
void WheelScrollBar::wheelEvent(QWheelEvent* e)
{
    switch (e->state()) {
        case Qt::ShiftButton:
            if (e->delta() > 0)
                emit actionTriggered(QAbstractSlider::SliderPageStepSub);
            else
                emit actionTriggered(QAbstractSlider::SliderPageStepAdd);
            break;

        case Qt::NoButton:
            if (e->delta() > 0)
                emit actionTriggered(QAbstractSlider::SliderSingleStepSub);
            else
                emit actionTriggered(QAbstractSlider::SliderSingleStepAdd);
            break;

        default:
            // do nothing
            break;
    }

    e->accept();
}


// vim: set sw=4 ts=4 tw=100:
