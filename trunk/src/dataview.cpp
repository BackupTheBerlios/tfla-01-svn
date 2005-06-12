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

#include <qframe.h>
#include <qlayout.h>
#include <qscrollbar.h>

#include "settings.h"
#include "dataview.h"
#include "dataplot.h"

using std::min;
using std::max;

#define SCROLL_SAMPLES_PER_LINE 3
#define LINESTEP_DIVISOR 5.0
#define MAX_SCROLL_VALUE 10000.0

// -------------------------------------------------------------------------------------------------
DataView::DataView(QWidget* parent, const char* name)
    throw ()
    : QWidget(parent, name), m_scrollDivisor(1)
{
    QVBoxLayout* layout = new QVBoxLayout(this, 0);
    m_dataPlot = new DataPlot(this, this);
    m_scrollBar = new QScrollBar(Qt::Horizontal, this);
    m_scrollBar->setTracking(false);
    
    layout->addWidget(m_dataPlot);
    layout->addWidget(m_scrollBar);
    
    connect(m_dataPlot,              SIGNAL(leftMarkerValueChanged(double)), 
            this,                    SIGNAL(leftMarkerValueChanged(double)));
    connect(m_dataPlot,              SIGNAL(rightMarkerValueChanged(double)), 
            this,                    SIGNAL(rightMarkerValueChanged(double)));
    connect(m_scrollBar,             SIGNAL(nextLine()),
            this,                    SLOT(navigateRight()));
    connect(m_scrollBar,             SIGNAL(prevLine()),
            this,                    SLOT(navigateLeft()));
    connect(m_scrollBar,             SIGNAL(sliderReleased()),
            this,                    SLOT(scrollValueChanged()));
    connect(m_scrollBar,             SIGNAL(nextPage()),
            this,                    SLOT(navigateRightPage()));
    connect(m_scrollBar,             SIGNAL(prevPage()),
            this,                    SLOT(navigateLeftPage()));
    connect(m_dataPlot,              SIGNAL(viewUpdated()),
            this,                    SLOT(updateScrollInfo()));
            
    // don't use this because of two reasons
    //  - if zoom is high, the user may not see all 
    //  - if zoom is low, it's slow
    //connect(m_scrollBar,             SIGNAL(sliderMoved(int)),
    //        this,                    SLOT(scrollValueChanged(int)));
    //connect(m_scrollBar,             SIGNAL(sliderPressed()),
    //        this,                    SLOT(scrollValueChanged()));
}


// -------------------------------------------------------------------------------------------------
void DataView::setData(const Data& data)
    throw ()
{
    // QScrollBar has a bug with very large values and large display width so that the
    // slider becomes too small (should be 90 %, is 5 %)
    // tested with values
    //   bar->setGeometry(0, 500, 500, 500);
    //   bar->setRange(0, 1727);
    //   bar->setPageStep(2962973);
    // to solve this, we connect to the pagestep / linestep signals directly and use a smaller
    // pagestep and range value here
    m_scrollDivisor =  m_currentData.bytes().size()  == 0
                       ? 1
                       : m_currentData.bytes().size() / 1000;
                       
    m_currentData = data;
    m_dataPlot->setStartIndex(0);
    m_dataPlot->updateData(true, true);
    m_dataPlot->clearMarkers();
}


// -------------------------------------------------------------------------------------------------
void DataView::zoomIn()
    throw ()
{
    if (m_dataPlot->getNumberOfDisplayedSamples() > 5)
    {
        m_dataPlot->setZoomFactor(m_dataPlot->getZoomFactor() * 2.0);
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::zoomOut()
    throw ()
{
    m_dataPlot->setZoomFactor(m_dataPlot->getZoomFactor() / 2.0);
}


// -------------------------------------------------------------------------------------------------
void DataView::zoomFit()
    throw ()
{
    if (m_currentData.bytes().size() != 0)
    {
        m_dataPlot->setZoomFactor( static_cast<double>(m_dataPlot->getCurrentWidthForPlot() - 1) / 
                              m_dataPlot->getPointsPerSample() / m_currentData.bytes().size() );
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::zoom1()
    throw ()
{
    m_dataPlot->setZoomFactor(1.0);
}


// -------------------------------------------------------------------------------------------------
void DataView::pos1() throw ()
{
    m_dataPlot->setStartIndex(0);
}


// -------------------------------------------------------------------------------------------------
void DataView::end() throw ()
{
    if (m_currentData.bytes().size() > 0)
    {
        m_dataPlot->setStartIndex(m_currentData.bytes().size() - 
                                  m_dataPlot->getNumberOfDisplayedSamples() + 2);
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::wheelEvent(QWheelEvent* e)
{
    if (e->delta() > 0)
    {
        zoomIn();
    }
    else
    {
        zoomOut();
    }
    updateScrollInfo();
    
    e->accept();
}


// -------------------------------------------------------------------------------------------------
void DataView::updateScrollInfo()
    throw ()
{
    int ps = m_dataPlot->getNumberOfDisplayedSamples();
    
    m_scrollBar->setRange(0, (m_currentData.bytes().size() - ps) / m_scrollDivisor);
    m_scrollBar->setValue(m_dataPlot->getStartIndex() / m_scrollDivisor);
    
    // set this to calculate the size
    if ((m_currentData.bytes().size() - ps) == 0)
    {
        m_scrollBar->setPageStep(0);
    }
    else
    {
        m_scrollBar->setPageStep(ps / m_scrollDivisor);
        m_scrollBar->setLineStep(qRound(ps / 10.0 / m_scrollDivisor)); 
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::redrawData()
    throw ()
{
    m_dataPlot->updateData(true);
}


// -------------------------------------------------------------------------------------------------
void DataView::scrollValueChanged()
    throw ()
{
    scrollValueChanged(m_scrollBar->value());
}


// -------------------------------------------------------------------------------------------------
void DataView::scrollValueChanged(int value) throw ()
{
    qDebug("value = %d", value);
    if (value == m_scrollBar->maxValue())
    {
        end();
    }
    else
    {
        m_dataPlot->setStartIndex(value * m_scrollDivisor);
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateLeft() throw ()
{
    m_dataPlot->setStartIndex(  max(0,   m_dataPlot->getStartIndex() -
                                        qRound(m_dataPlot->getNumberOfDisplayedSamples() / 10.0)) );
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateRight() throw ()
{
    m_dataPlot->setStartIndex(  min(  m_dataPlot->getStartIndex() +
                                         qRound(m_dataPlot->getNumberOfDisplayedSamples() / 10.0),
                                      static_cast<int>(m_currentData.bytes().size())) );
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateLeftPage() throw ()
{
    m_dataPlot->setStartIndex(  max(0,   m_dataPlot->getStartIndex() -
                                        qRound(m_dataPlot->getNumberOfDisplayedSamples())) );
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateRightPage() throw ()
{
    m_dataPlot->setStartIndex(  min(  m_dataPlot->getStartIndex() +
                                         qRound(m_dataPlot->getNumberOfDisplayedSamples()),
                                      static_cast<int>(m_currentData.bytes().size())) );
}


// -------------------------------------------------------------------------------------------------
void DataView::jumpToLeftMarker() throw ()
{
    if (m_dataPlot->getLeftMarker() >= 0)
    {
        m_dataPlot->setStartIndex(max(0, m_dataPlot->getLeftMarker()));
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::jumpToRightMarker() throw ()
{
    if (m_dataPlot->getRightMarker() >= 0)
    {
        // 3 instead of 2 because in end() size() is lastElementIndex + 1
        m_dataPlot->setStartIndex(max(0, m_dataPlot->getRightMarker() -
                                       m_dataPlot->getNumberOfDisplayedSamples() + 3)  );
    }
}

