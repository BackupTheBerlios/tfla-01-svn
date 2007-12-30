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
#include <qpixmap.h>
#include <qpainter.h>
#include <qfontmetrics.h>
#include <qevent.h>
#include <qapplication.h>
#include <qstatusbar.h>

#include "dataplot.h"
#include "global.h"
#include "settings.h"
#include "dataview.h"
#include "tfla01.h"

// -------------------------------------------------------------------------------------------------
#define DEFAULT_POINTS_PER_SAMPLE 10

/* for detect too many samples per pixel
 * Change mode of plot, condense samples in two pixels (begin-end) or plot gliches only once
 */
#define MAXIMUM_POINTS_PER_LINE	(2*4096)


// -------------------------------------------------------------------------------------------------
DataPlot::DataPlot(QWidget* parent, DataView* dataView, const char* name) throw ()
    : QWidget(parent, name, WRepaintNoErase), 
      m_dataView(dataView),  m_zoomFactor(1.0),m_startIndex(0),
       m_SampleLeftMarker(-1), m_SampleRightMarker(-1),m_lastPixmap(0, 0)
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
void DataPlot::setZoomFactor(double factor) throw ()
{
    m_zoomFactor = factor;
    recalculateXPositions();
    updateData(true);
}


// -------------------------------------------------------------------------------------------------
double DataPlot::getZoomFactor() const 
{
    return m_zoomFactor;
}


// -------------------------------------------------------------------------------------------------
void DataPlot::setStartIndex(sample_time_t startIndex) throw ()
{
    m_startIndex = startIndex;
    if (!m_xPositions.isEmpty() && startIndex != m_xPositions.first().spos )
    {
        updateData(true,true);
    }
    else
    {
        updateData(true,false);
    }
}


// -------------------------------------------------------------------------------------------------
sample_time_t DataPlot::getStartIndex() const
{
    return m_startIndex;
}

sample_time_t DataPlot::getSampleNr(int x_position)
{
	if (x_position < 0)
	{
		return(0);
	}
	else if ((unsigned)x_position >= m_xPositions.size() )
	{
		if (m_dataView->m_currentData.DataLoaded())
		{
			return(m_dataView->m_currentData.getNumSamples());
		}
	}
	return(m_xPositions[x_position].spos);
}


// -------------------------------------------------------------------------------------------------
sample_time_t DataPlot::getLeftMarker() const throw ()
{
    return m_SampleLeftMarker;
}


// -------------------------------------------------------------------------------------------------
void DataPlot::setLeftMarker(sample_time_t markerpos)  throw ()
{
    double ms = m_dataView->m_currentData.getMsecsForSample(markerpos);
    
    if (markerpos == -1 || ms >= 0.0)
    {
        m_SampleLeftMarker = markerpos;
        updateData(false);
        leftMarkerValueChanged(ms);
    }
    else
    {
        static_cast<Tfla01*>(qApp->mainWidget())->statusBar()->message(   
            tr("Point is outside of data area."), 4000); 
    }
}


// -------------------------------------------------------------------------------------------------
sample_time_t DataPlot::getRightMarker() const throw ()
{
    return m_SampleRightMarker;
}


// -------------------------------------------------------------------------------------------------
void DataPlot::setRightMarker(sample_time_t  markerpos) throw ()
{
    double ms = m_dataView->m_currentData.getMsecsForSample(markerpos);
    
    if (markerpos == -1 || ms >= 0.0)
    {
        m_SampleRightMarker = markerpos;
        updateData(false);
        rightMarkerValueChanged(ms);
    }
    else
    {
        static_cast<Tfla01*>(qApp->mainWidget())->statusBar()->message(   
            tr("Point is outside of data area."), 4000); 
    }
}


// -------------------------------------------------------------------------------------------------
void DataPlot::clearMarkers() throw ()
{
    setLeftMarker(-1);
    setRightMarker(-1);
    updateData(false);
}


// -------------------------------------------------------------------------------------------------
sample_time_t DataPlot::getNumberOfDisplayedSamples () const throw ()
{
    sample_time_t dataToDisplayMax = m_dataView->m_currentData.getNumSamples() - m_startIndex;
    sample_time_t ScreenSpace = getNumberOfPossiblyDisplayedSamples();
    return  (dataToDisplayMax > ScreenSpace) 
          ? ScreenSpace
          : dataToDisplayMax;
}


// -------------------------------------------------------------------------------------------------
sample_time_t DataPlot::getNumberOfPossiblyDisplayedSamples() const throw()
{
    int ret = m_xPositions.size();
    
    // last sample at ret-2 because we have one outside the draw area
    return ret < 3
        ? 0
        : m_xPositions[ret-2].spos - m_xPositions.first().spos;
}


// -------------------------------------------------------------------------------------------------
int DataPlot::getCurrentWidthForPlot() const throw ()
{
    return width() - static_cast<int>( 2.0 / 3.0 * (height() / NUMBER_OF_WIRE_PER_SAPMPLE) );
}


// -------------------------------------------------------------------------------------------------
int DataPlot::getPointsPerSample(double zoom ) const throw ()
{
    return static_cast<int>(DEFAULT_POINTS_PER_SAMPLE * zoom);
}

// -------------------------------------------------------------------------------------------------
int DataPlot::getSampleOnScreenPosition(sample_time_t  pos)
{
	if (valid_sample_pos(pos))
	{
		if (m_xPositions.empty())
		{
			return(-1);
		}
		int top=m_xPositions.size();  
		if (top < 2 || pos < m_xPositions.first().spos || pos > m_xPositions.last().spos)
		{
			return(-1);
		}
		// linear search (faste will be bisection search) 
		for (int k=0; k<top; k+=1)
		{
			if (pos <= m_xPositions[k].spos  )
			{
				return(k);
			}
		}
	}
	return(-1);
}

// -------------------------------------------------------------------------------------------------
void DataPlot::recalculateXPositions() throw ()
{
    int leftBegin = getLeftBegin();
    
    // calculate the X positions -------------------------------------------------------------------
    if (m_dataView->m_currentData.DataLoaded())
    {
        m_xPositions.clear();
        sample_time_t tp = 0;m_startIndex;
        int w = width();
        int sample_per_point;
        PLOT_POINT currentX; 
        
        if (DEFAULT_POINTS_PER_SAMPLE * m_zoomFactor < 1.0)
        {
        	sample_per_point = qRound(1.0/(DEFAULT_POINTS_PER_SAMPLE * m_zoomFactor));
        }
        else
        {
        	sample_per_point=1;
        }
        currentX.x = leftBegin;
        currentX.spos = tp+m_startIndex;
        m_xPositions.push_back(currentX);
        tp +=sample_per_point;
        while (currentX.x < w)
        {
            currentX.x = qRound(leftBegin + DEFAULT_POINTS_PER_SAMPLE * m_zoomFactor * tp);
            currentX.spos = tp+m_startIndex;
            m_xPositions.push_back(currentX);
            tp+=sample_per_point;
        }
        
        // last because to have one point outside the draw area
        currentX.x = qRound(leftBegin + DEFAULT_POINTS_PER_SAMPLE * m_zoomFactor * tp );
        currentX.spos =tp+m_startIndex;
        m_xPositions.push_back(currentX);
    }
    else
    {
        m_xPositions.clear();
    }
}




// -------------------------------------------------------------------------------------------------
QPixmap DataPlot::getScreenshot() throw ()
{
    QPixmap ret = m_lastPixmap;
    QPainter p(&ret);
    drawMarkers(&p);
    p.end();
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
            || (m_lastPixmap.width() == 0 && m_lastPixmap.height() == 0))
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
int DataPlot::getLeftBegin() const throw ()
{
    return static_cast<int>( 2.0 / 3.0 * height() / NUMBER_OF_WIRE_PER_SAPMPLE );
}


// -------------------------------------------------------------------------------------------------
void DataPlot::plot(QPainter* painter)
    throw ()
{
	int i,kk,x;
	int heightPerField = height() / NUMBER_OF_WIRE_PER_SAPMPLE;
//    ByteVector data = m_dataView->m_currentData.bytes();
    sample_time_t NumSamples = m_dataView->m_currentData.getNumSamples();
//    bool sample_condesed = (NumSamples >)
    // set font ------------------------------------------------------------------------------------
    QFont textFont;
	QFontMetrics fm(textFont);
	kk = textFont.pointSize();
	i   = fm.height();
	textFont.setPointSize(kk*heightPerField/i);
//  textFont.setPointSize(textFont.pointSize()*2);

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
		QString lab;
        for (int i = 0; i < NUMBER_OF_WIRE_PER_SAPMPLE ; i++)
        {
			if (i != NUMBER_OF_WIRE_PER_SAPMPLE-1)
			{
				painter->setPen(linePen);
				painter->drawLine(0, current, width(), current);
			}
			
            painter->setPen(textPen);
            painter->drawText(10, current - heightPerField / 3,lab.number(i+1));
            current += heightPerField;
        }
//        painter->setPen(textPen);
//        painter->drawText(10, current - 15, QString::number(NUMBER_OF_WIRE_PER_SAPMPLE));
    }
    
    // draw vertical lines ("grid") ----------------------------------------------------------------
    if (m_xPositions.size() >= 2 && (m_xPositions[1].x - m_xPositions[0].x) > 3)
    {
        /* Draw Vertical lines for magnificient samples */
    	painter->setPen(gridPen);
          
        for (QValueVector<PLOT_POINT>::iterator it = m_xPositions.begin(); it != m_xPositions.end(); ++it)
        {
        	x=it->x; 
            painter->drawLine(x, 0,x, height());
        }
    }
    
    // draw the lines
    if (NumSamples > 1)
    {
        painter->setPen(dataPen);
        int currentY = 4 * heightPerField / 5;
        int lastXOnScreen = m_xPositions.last().x;
        int firstXOnScreen = m_xPositions.first().x;
        
        for (int i = 0; i < NUMBER_OF_WIRE_PER_SAPMPLE; i++, currentY += heightPerField)
        {
            int currentLowY = currentY;
            int currentHighY = currentY - 3 * heightPerField / 5;
            
            Data::LineState ls = m_dataView->m_currentData.getLineState(i);
            
            switch (ls)
            {
                case Data::LS_ALWAYS_L:
                    painter->drawLine(firstXOnScreen, currentLowY, 
                                      lastXOnScreen, currentLowY);
                    break;
                    
                case Data::LS_ALWAYS_H:
                    painter->drawLine(firstXOnScreen, currentHighY, 
                                      lastXOnScreen, currentHighY);
                    break;
                    
                case Data::LS_CHANGING:
                {
                    QPointArray points(m_xPositions.size());
                    
                    QValueVector<PLOT_POINT>::iterator it = m_xPositions.begin();
                    uint j0 = 0;
                    sample_time_t tp;
                    uint xp,xk,glich;
                    bool newHigh,oldHigh;
                    // handle the first point
                    
                    tp =it->spos;
                    oldHigh = m_dataView->m_currentData.GetWire(tp,i);
                    newHigh = oldHigh;
                    points.setPoint(j0++, it->x, oldHigh ? currentHighY : currentLowY);
                    ++it;

                    while (it != m_xPositions.end() && tp < NumSamples)
                    {
                        glich =  m_dataView->m_currentData.ScanWire(tp,it->spos-tp,i);
                        if (glich & 1)
                        {
                        	newHigh = !oldHigh;
                    		oldHigh = newHigh;
                        }
                        tp = it->spos;
                        if (glich)
                        {
                        	xp=(it - 1)->x;
                        	xk=it->x;
                        	if ( glich == 1)
                        	{
                        		points.setPoint(j0++, xp, !newHigh ? currentHighY : currentLowY);
                        		points.setPoint(j0++, xk,  newHigh ? currentHighY : currentLowY);
                        	}
                        	else
                        	{
                        		if ((glich & 1) == 0)
                        		{
                            		points.setPoint(j0++, xp, newHigh ? currentHighY : currentLowY);
                        		}
                        		points.setPoint(j0++, xp, !newHigh ? currentHighY+4 : currentLowY+4);
                        		points.setPoint(j0++, xk, newHigh ? currentHighY : currentLowY);
                        	}
                        }
                        ++it;
                    }
                    
                    // end handling
                    points.setPoint(j0++, (it - 1)->x, newHigh ? currentHighY : currentLowY);
                    
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
    
    if (valid_sample_pos(m_SampleLeftMarker) )
    {
        int displaySample =  getSampleOnScreenPosition(m_SampleLeftMarker);
        
        if (displaySample >= 0)
        {
            int x = m_xPositions[displaySample].x;
            painter->setPen(leftMarkerPen);
            painter->drawLine(x, 0, x, height());
        }
    }
    if (valid_sample_pos(m_SampleRightMarker) )
    {
        int displaySample = getSampleOnScreenPosition(m_SampleRightMarker);
        
        if (displaySample >= 0 )
        {
            int x = m_xPositions[displaySample].x;
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
        if (!m_dataView->m_currentData.DataLoaded() || m_xPositions.size()<3)
        {
            return;
        }
//        SampleVector data_x = m_dataView->m_currentData.bytes();
        
        // check if there is data displayed
        int rsample = m_xPositions.size()-2;
        int mouse_pos = evt->x();
        if (mouse_pos+5 < m_xPositions.first().x || mouse_pos-5 > m_xPositions[rsample].x)
        {
        	return;
        }
        double NormDistance  = (mouse_pos - m_xPositions[0].x);
        NormDistance /= (m_xPositions[rsample].x - m_xPositions[0].x);
        
        // normalize
        sample_time_t sample = m_xPositions[qRound(NormDistance * rsample)].spos;
        if (sample<0 || sample > m_dataView->m_currentData.getNumSamples())
        {
        	return;
        }
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

