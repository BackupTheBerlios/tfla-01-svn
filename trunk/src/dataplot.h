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

#include <q3frame.h>
#include <q3valuevector.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPaintEvent>

class DataView;

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
        double getZoomFactor() const throw ();
        int getNumberOfDisplayedSamples() const throw ();
        int getNumberOfPossiblyDisplayedSamples() const throw();
        int getCurrentWidthForPlot() const throw ();
        int getPointsPerSample(double zoom = 1.0) const throw ();
        QPixmap getScreenshot() throw ();

        // start index ----------------------------------------------------------------------------
        void setStartIndex(int startIndex) throw ();
        int getStartIndex() const throw ();

        // marker handling ------------------------------------------------------------------------
        int getLeftMarker() const throw ();
        void setLeftMarker(int markerPosition) throw ();

        int getRightMarker() const throw ();
        void setRightMarker(int markerPosition) throw ();

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
        int                 m_startIndex;
        double              m_zoomFactor;
        QPixmap             m_lastPixmap;
        int                 m_lastWidth;
        int                 m_lastHeight;
        bool                m_gridEnabled;
        Q3ValueVector<uint>  m_xPositions;
        int                 m_leftMarker;
        int                 m_rightMarker;
};

#endif /* DATAPLOT_H */

// vim: set sw=4 ts=4 tw=100:
