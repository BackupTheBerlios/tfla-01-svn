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
#ifndef WHEELSCROLLBAR_H
#define WHEELSCROLLBAR_H

#include <qscrollbar.h>


class WheelScrollBar : public QScrollBar
{
    Q_OBJECT
    
    public:
        WheelScrollBar(QWidget * parent, const char* name = 0);
        WheelScrollBar(Orientation orientation, QWidget* parent, const char* name = 0);
        WheelScrollBar(int minValue, int maxValue, int lineStep, int pageStep, int value, 
                       Orientation orientation, QWidget * parent, const char* name = 0);
    
    protected:
        void wheelEvent(QWheelEvent* e);
};

#endif /* WHEELSCROLLBAR_H */
