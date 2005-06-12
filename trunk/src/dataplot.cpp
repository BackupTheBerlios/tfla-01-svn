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
#include <algorithm>

#include <qpixmap.h>
#include <qpainter.h>
#include <qevent.h>

#include "dataplot.h"
#include "global.h"
#include "settings.h"
#include "dataview.h"

using std::min;
using std::max;

// -------------------------------------------------------------------------------------------------
#define DEFAULT_POINTS_PER_SAMPLE 10

// -------------------------------------------------------------------------------------------------
DataPlot::DataPlot(QWidget* parent, DataView* dataView, const char* name)
    throw ()
    : QWidget(parent, name, WRepaintNoErase), 
      m_dataView(dataView), m_startIndex(0), m_zoomFactor(1.0),
      m_lastPixmap(0, 0), m_leftMarker(-1), m_rightMarker(-1)
{
    setFocusPolicy(QWidget::WheelFocus);
    setBackgroundMode( QWidget::NoBackground );
}


// -------------------------------------------------------------------------------------------------
void DataPlot::paintEvent(QPaintEvent*)
{
    updateData(false);
}


// -------------------------------------------------------------------------------------------------
void DataPlot::setZoomFactor(double factor)
    throw ()
{
    m_zoomFactor = factor;
    recalculateXPositions();
    updateData(true);
}


// -------------------------------------------------------------------------------------------------
double DataPlot::getZoomFactor() const
    throw ()
{
    return m_zoomFactor;
}


// -------------------------------------------------------------------------------------------------
void DataPlot::setStartIndex(int startIndex)
    throw ()
{
    m_startIndex = startIndex;
    updateData(true);
}


// -------------------------------------------------------------------------------------------------
int DataPlot::getStartIndex() const
    throw ()
{
    return m_startIndex;
}


// -------------------------------------------------------------------------------------------------
int DataPlot::getLeftMarker() const throw ()
{
    return m_leftMarker;
}


// -------------------------------------------------------------------------------------------------
void DataPlot::setLeftMarker(int markerpos)  throw ()
{
    m_leftMarker = markerpos;
    updateData(false);
    leftMarkerValueChanged( m_dataView->m_currentData.getMsecsForSample(markerpos) );
}


// -------------------------------------------------------------------------------------------------
int DataPlot::getRightMarker() const throw ()
{
    return m_rightMarker;
}


// -------------------------------------------------------------------------------------------------
void DataPlot::setRightMarker(int markerpos) throw ()
{
    ByteVector data = m_dataView->m_currentData.bytes();
    m_rightMarker = markerpos;
    updateData(false);
    rightMarkerValueChanged( m_dataView->m_currentData.getMsecsForSample(markerpos) );
}


// -------------------------------------------------------------------------------------------------
void DataPlot::clearMarkers() throw ()
{
    setLeftMarker(-1);
    setRightMarker(-1);
    updateData(false);
}


// -------------------------------------------------------------------------------------------------
int DataPlot::getNumberOfDisplayedSamples () const
    throw ()
{
    return m_xPositions.size();
}


// -------------------------------------------------------------------------------------------------
int DataPlot::getCurrentWidthForPlot() const
    throw ()
{
    return width() - static_cast<int>( 2.0 / 3.0 * (height() / NUMBER_OF_BITS_PER_BYTE) );
}


// -------------------------------------------------------------------------------------------------
int DataPlot::getPointsPerSample(double zoom ) const throw ()
{
    return static_cast<int>(DEFAULT_POINTS_PER_SAMPLE * zoom);
}


// -------------------------------------------------------------------------------------------------
QPixmap DataPlot::getScreenshot() throw ()
{
    QPixmap ret = m_lastPixmap;
    ret.resize(width(), height());
    
    return ret;
}


// -------------------------------------------------------------------------------------------------
void DataPlot::updateData(bool forceRedraw, bool forceRecalculatePositions) throw ()
{
    QPainter p;
    QPixmap screenPixmap( static_cast<int>(  width() + DEFAULT_POINTS_PER_SAMPLE *  m_zoomFactor),
                         height() );
    
    if (m_lastWidth != width() || forceRecalculatePositions)
    {
        recalculateXPositions();
    }
    
    if (forceRedraw || m_lastWidth != width() || m_lastHeight != height() 
            || !(m_lastPixmap.width() == 0 && m_lastPixmap.height() == 0))
    {
        // larger pixmap to have space to draw the last point
        m_lastPixmap.resize( static_cast<int>(  width() + DEFAULT_POINTS_PER_SAMPLE * 
                                                          m_zoomFactor),
                             height() );
        m_lastPixmap.fill(QColor(Settings::set().readEntry("UI/Background_Color")));
        
        p.begin(&m_lastPixmap);
        plot(&p);
        p.end();
        
        m_lastHeight = height();
        m_lastWidth = width();
        
        emit viewUpdated();
    }
    
    p.begin(&screenPixmap);
    p.drawPixmap(0, 0, m_lastPixmap);
    drawMarkers(&p);
    p.end();
    
    p.begin(this);
    p.drawPixmap(0, 0, screenPixmap); 
    p.end();
}


// -------------------------------------------------------------------------------------------------
void DataPlot::recalculateXPositions() throw ()
{
    m_xPositions.clear();
    int leftBegin = getLeftBegin();
    
    // calculate the X positions -------------------------------------------------------------------
    if (m_dataView->m_currentData.bytes().size() > 0)
    {
        int i = 0;
        int max = m_dataView->m_currentData.bytes().size() - 1;
        int currentX = leftBegin;
        while (currentX < width() && i < max)
        {
            currentX = qRound(leftBegin + DEFAULT_POINTS_PER_SAMPLE * m_zoomFactor * i);
            m_xPositions.push_back(currentX);
            
            i++;
        }
        
        // last because to have one point outside the draw area
        currentX = static_cast<int>(leftBegin + DEFAULT_POINTS_PER_SAMPLE * m_zoomFactor * i);
        m_xPositions.push_back(currentX);
    }
}


// -------------------------------------------------------------------------------------------------
int DataPlot::getLeftBegin() const throw ()
{
    return static_cast<int>( 2.0 / 3.0 * height() / NUMBER_OF_BITS_PER_BYTE );
}


// -------------------------------------------------------------------------------------------------
void DataPlot::plot(QPainter* painter)
    throw ()
{
    int heightPerField = height() / NUMBER_OF_BITS_PER_BYTE;
    ByteVector data = m_dataView->m_currentData.bytes();
    
    // set font ------------------------------------------------------------------------------------
    QFont textFont;
    textFont.setPixelSize(heightPerField / 2);
    painter->setFont(textFont);
    
    // try to recalculate x positions --------------------------------------------------------------
    if (m_xPositions.size() == 0)
    {
        recalculateXPositions();
    }
    
    // set the needed pens -------------------------------------------------------------------------
    QPen linePen = QPen(QColor(100, 100, 100), 2, Qt::SolidLine);
    QPen textPen = QPen(QColor(255, 255, 255), 1, Qt::SolidLine);
    QPen gridPen = QPen(QColor(100, 100, 100), 1, Qt::SolidLine);
    QPen dataPen = QPen(Settings::set().readEntry("UI/Foreground_Color_Line"), 2, Qt::SolidLine);
    
    // draw the fields and the text ----------------------------------------------------------------
    {
        int current = heightPerField;
        for (int i = 0; i < (NUMBER_OF_BITS_PER_BYTE - 1); i++)
        {
            painter->setPen(linePen);
            painter->drawLine(0, current, width(), current);
            
            painter->setPen(textPen);
            painter->drawText(10, current - heightPerField / 3, QString::number(i+1));
            current += heightPerField;
        }
        painter->setPen(textPen);
        painter->drawText(10, current - 15, QString::number(NUMBER_OF_BITS_PER_BYTE));
    }
    
    // draw vertical lines ("grid") ----------------------------------------------------------------
    if (m_xPositions.size() >= 2 && (m_xPositions[1] - m_xPositions[0]) > 3)
    {
        painter->setPen(gridPen);
        
        for (QValueVector<uint>::iterator it = m_xPositions.begin(); it != m_xPositions.end(); ++it)
        {
            painter->drawLine(*it, 0, *it, height());
        }
    }
    
    // draw the lines
    if (data.size() != 0)
    {
        painter->setPen(dataPen);
        int currentY = 4 * heightPerField / 5;
        
        for (int i = 0; i < NUMBER_OF_BITS_PER_BYTE; i++, currentY += heightPerField)
        {
            int currentLowY = currentY;
            int currentHighY = currentY - 3 * heightPerField / 5;
            
            Data::LineState ls = m_dataView->m_currentData.getLineState(i);
            
            switch (ls)
            {
                case Data::LS_ALWAYS_L:
                    painter->drawLine(m_xPositions.first(), currentLowY, 
                                      m_xPositions.last(), currentLowY);
                    break;
                    
                case Data::LS_ALWAYS_H:
                    painter->drawLine(m_xPositions.first(), currentHighY, 
                                      m_xPositions.last(), currentHighY);
                    break;
                    
                case Data::LS_CHANGING:
                {
                    QPointArray points(m_xPositions.size());
                    
                    QValueVector<uint>::iterator it = m_xPositions.begin();
                    uint j = m_startIndex, j0 = 0;
                    
                    // handle the first point
                    bool oldHigh = data[j] & (1 << i);
                    bool newHigh = oldHigh;
                    points.setPoint(j0++, *it, oldHigh ? currentHighY : currentLowY);
                    ++it;
                    
                    while (it != m_xPositions.end() && j < (data.size()))
                    {
                        newHigh = data[j] & (1 << i);
                        
                        if (newHigh != oldHigh)
                        {
                            points.setPoint(j0++, *(it - 1), oldHigh ? currentHighY : currentLowY);
                            points.setPoint(j0++, *it,       newHigh ? currentHighY : currentLowY);
                            
                            oldHigh = newHigh;
                        }
                        ++it, ++j;
                    }
                    
                    // end handling
                    points.setPoint(j0++, *(it - 1), newHigh ? currentHighY : currentLowY);
                    
                    painter->drawPolyline(points, 0, j0);
                    break;
                }
            }
        }
    }
}


// -------------------------------------------------------------------------------------------------
void DataPlot::drawMarkers(QPainter* painter) throw ()
{
    QPen leftMarkerPen = QPen(Settings::set().readEntry("UI/Left_Marker_Color"), 2, Qt::SolidLine);
    QPen rightMarkerPen = QPen(Settings::set().readEntry("UI/Right_Marker_Color"), 2, Qt::SolidLine);
    
    if (m_leftMarker != -1)
    {
        int displaySample = m_leftMarker - m_startIndex;
        
        if (displaySample >= 0 && displaySample < static_cast<int>(m_xPositions.size()))
        {
            int x = m_xPositions[displaySample];
            painter->setPen(leftMarkerPen);
            painter->drawLine(x, 0, x, height());
        }
    }
    if (m_rightMarker != -1)
    {
        int displaySample = m_rightMarker - m_startIndex;
        
        if (displaySample >= 0 && displaySample < static_cast<int>(m_xPositions.size()))
        {
            int x = m_xPositions[displaySample];
            painter->setPen(rightMarkerPen);
            painter->drawLine(x, 0, x, height());
        }
    }
}


// -------------------------------------------------------------------------------------------------
void DataPlot::mousePressEvent(QMouseEvent* evt)
{
    if (evt->button() == Qt::LeftButton || evt->button() == Qt::RightButton)
    {
        ByteVector data = m_dataView->m_currentData.bytes();
        
        // check if there is data displayed
        if (data.size() <= 0)
        {
            return;
        }
        
        double leftDistance = evt->x() - getLeftBegin();
        
        // user clicked in the label area
        if (leftDistance < 0)
        {
            return;
        }
        
        // check if the user clicked too much right
        if (evt->x() > static_cast<int>(m_xPositions.last()))
        {
            return;
        }
        
        double possibleSamplesPerScreen = static_cast<double>(width()) / 
                                          (DEFAULT_POINTS_PER_SAMPLE * m_zoomFactor);
        
        int sample = qRound(leftDistance * possibleSamplesPerScreen / width() + getStartIndex());
        
        if (evt->button() == Qt::LeftButton)
        {
            setLeftMarker(sample);
        }
        else
        {
            setRightMarker(sample);
        }
    }
}

