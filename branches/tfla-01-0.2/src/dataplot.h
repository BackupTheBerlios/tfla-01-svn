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
#ifndef DATAPLOT_H
#define DATAPLOT_H

#include <qframe.h>
#include <qvaluevector.h>
#include <qpixmap.h>
#include "global.h"

class DataView;

typedef struct
{
	int  x;		/* Screen position  */
	sample_time_t spos;	/* First Sample Time */
} PLOT_POINT;


class DataPlot : public QWidget
{
    Q_OBJECT
    
    public:
        enum DrawState { DS_START, DS_LOW, DS_HIGH };
    
    public:
    
        DataPlot(QWidget* parent, DataView* dataView, const char* name = 0) throw ();
        virtual ~DataPlot() {}
        
        void updateData(bool forceRedraw, bool forceRecalculatePositions = false) throw ();
        
        void setZoomFactor(double factor) throw ();
        double getZoomFactor() const ;
        sample_time_t getNumberOfDisplayedSamples() const throw ();
        sample_time_t getNumberOfPossiblyDisplayedSamples() const throw();
        int getCurrentWidthForPlot() const throw ();
        int getPointsPerSample(double zoom = 1.0) const throw ();
        sample_time_t getSampleNr(int x_position);
        int getSampleOnScreenPosition(sample_time_t sample_nr); /* -1 below or above  screen area */ 
        
        QPixmap getScreenshot() throw ();
        
        // start index ----------------------------------------------------------------------------
        void setStartIndex(sample_time_t startIndex) throw (); 
        sample_time_t getStartIndex() const;
        
        // marker handling ------------------------------------------------------------------------
        sample_time_t getLeftMarker() const throw ();
        void setLeftMarker(sample_time_t markerPosition) throw ();
        
        sample_time_t getRightMarker() const throw ();
        void setRightMarker(sample_time_t markerPosition) throw ();
        
        void clearMarkers() throw ();
        
    signals:
        void leftMarkerValueChanged(double value);
        void rightMarkerValueChanged(double value);
        
    protected:
        void plot(QPainter* painter) throw ();
        void drawMarkers(QPainter* painter) throw ();
        void recalculateXPositions() throw ();
        int getLeftBegin() const throw ();
        
        void paintEvent(QPaintEvent* evt);
        void mousePressEvent(QMouseEvent* evt);
        
    signals:    
        void viewUpdated();
        
    private:
        DataView*           m_dataView;
        double              m_zoomFactor; /* scale */
        long long           m_startIndex; /* Index to first sample to display */
        long long		    m_stopIndex;   /* index to last sample to display  */
        long long           m_SampleLeftMarker;		/* sample_NR for left marker  */
        long long           m_SampleRightMarker;     /* sample_NR for right marker */
        
        QValueVector<PLOT_POINT>  m_xPositions; /* array mapping smaples to screen (X) */
                                                /* starting from m_start_index         */
        QPixmap             m_lastPixmap;
        int                 m_lastWidth;
        int                 m_lastHeight;
        bool                m_gridEnabled;      /* sample/raster < 1          */
};

#endif /* DATAPLOT_H */
