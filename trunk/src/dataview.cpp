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
#include <Q3Frame>
#include <QLayout>
#include <QPixmap>
#include <QApplication>
#include <QScrollBar>
#include <Q3FileDialog>
#include <QStatusBar>
#include <QMessageBox>
#include <QCursor>
#include <Q3TextStream>
#include <QWheelEvent>
#include <Q3VBoxLayout>

#include "settings.h"
#include "dataview.h"
#include "dataplot.h"
#include "tfla01.h"
#include "exportdialog.h"

#define SCROLL_SAMPLES_PER_LINE 3
#define LINESTEP_DIVISOR 5.0
#define MAX_SCROLL_VALUE 10000.0

// -------------------------------------------------------------------------------------------------
DataView::DataView(QWidget* parent, const char* name)
    throw ()
    : QWidget(parent, name)
    , m_scrollDivisor(1)
{
    Q3VBoxLayout* layout = new Q3VBoxLayout(this, 0);
    m_dataPlot = new DataPlot(this, this, "DataPlot");
    m_scrollBar = new WheelScrollBar(Qt::Horizontal, this, "WheelScrollBar");
    m_scrollBar->setTracking(false);

    layout->addWidget(m_dataPlot);
    layout->addWidget(m_scrollBar);

    connect(m_dataPlot,              SIGNAL(leftMarkerValueChanged(double)),
            this,                    SIGNAL(leftMarkerValueChanged(double)));
    connect(m_dataPlot,              SIGNAL(rightMarkerValueChanged(double)),
            this,                    SIGNAL(rightMarkerValueChanged(double)));
    connect(m_scrollBar,             SIGNAL(actionTriggered(int)),
            this,                    SLOT(navigateRightPage()));
    connect(m_scrollBar,             SIGNAL(sliderReleased()),
            this,                    SLOT(scrollValueChanged()));
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
    m_currentData = data;
    m_scrollDivisor =  m_currentData.bytes().size()  == 0
                       ? 1
                       : m_currentData.bytes().size() / 1000;

    m_dataPlot->setStartIndex(0);
    m_dataPlot->update();
    m_dataPlot->clearMarkers();
}


// -------------------------------------------------------------------------------------------------
void DataView::zoomIn()
    throw ()
{
    if (m_dataPlot->getNumberOfDisplayedSamples() > 5)
        m_dataPlot->setZoomFactor(m_dataPlot->getZoomFactor() * 2.0);
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
    if (m_currentData.bytes().size() != 0) {
        m_dataPlot->setStartIndex(0);
        m_dataPlot->setZoomFactor( static_cast<double>(m_dataPlot->getCurrentWidthForPlot() - 1) /
                              m_dataPlot->getPointsPerSample() / m_currentData.bytes().size() );
    } else {
        static_cast<Tfla01*>(qApp->mainWidget())->statusBar()->message(
            tr("Function only available if data is displayed."), 4000);
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::zoom1() throw ()
{
    m_dataPlot->setZoomFactor(1.0);
}


// -------------------------------------------------------------------------------------------------
void DataView::zoomMarkers() throw ()
{
    if (m_dataPlot->getLeftMarker() > 0 && m_dataPlot->getRightMarker() > 0) {
        double diff = DABS(m_dataPlot->getRightMarker() - m_dataPlot->getLeftMarker());
        m_dataPlot->setZoomFactor( static_cast<double>(m_dataPlot->getCurrentWidthForPlot() - 2) /
                              m_dataPlot->getPointsPerSample() / diff );
        m_dataPlot->setStartIndex(QMIN(m_dataPlot->getLeftMarker(), m_dataPlot->getRightMarker()));
    } else {
        static_cast<Tfla01*>(qApp->mainWidget())->statusBar()->message(
            tr("Function only available if both markers are set."), 4000);
    }
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
        m_dataPlot->setStartIndex(m_currentData.bytes().size() -
                                  m_dataPlot->getNumberOfPossiblyDisplayedSamples() + 2);
}


// -------------------------------------------------------------------------------------------------
void DataView::wheelEvent(QWheelEvent* e)
{
    switch (e->state()) {
        case Qt::ControlButton:
            if (e->delta() > 0)
                zoomIn();
            else
                zoomOut();
            break;

        case Qt::ShiftButton:
            if (e->delta() > 0)
                navigateLeftPage();
            else
                navigateRightPage();
            break;

        case Qt::NoButton:
            if (e->delta() > 0)
                navigateLeft();
            else
                navigateRight();
            break;

        default:
            // do nothing
            break;
    }

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
        m_scrollBar->setPageStep(0);
    else
        m_scrollBar->setPageStep(ps / m_scrollDivisor);
        m_scrollBar->setLineStep(qRound(ps / 10.0 / m_scrollDivisor));

#if 0
        PRINT_TRACE("div = %d", m_scrollDivisor);
        PRINT_TRACE("range = %d to %d", m_scrollBar->minValue(), m_scrollBar->maxValue());
        PRINT_TRACE("ps = %d", m_scrollBar->pageStep());
#endif
}


// -------------------------------------------------------------------------------------------------
void DataView::redrawData()
    throw ()
{
    m_dataPlot->update();
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
    if (value == m_scrollBar->maxValue())
        end();
    else
        m_dataPlot->setStartIndex(value * m_scrollDivisor);
}


// -------------------------------------------------------------------------------------------------
void DataView::navigate(QAbstractSlider::SliderAction direction) throw ()
{
    int si;

    switch (direction) {
        case QAbstractSlider::SliderSingleStepAdd:
            m_dataPlot->setStartIndex( QMIN(m_dataPlot->getStartIndex() +
                        qRound(m_dataPlot->getNumberOfPossiblyDisplayedSamples() / 10.0),
                        int(m_currentData.bytes().size())) );
            break;

        case QAbstractSlider::SliderSingleStepSub:
            m_dataPlot->setStartIndex( QMAX(0,  m_dataPlot->getStartIndex() -
                        qRound(m_dataPlot->getNumberOfPossiblyDisplayedSamples() / 10.0)) );
            break;

        case QAbstractSlider::SliderPageStepAdd:
            si = QMIN( m_dataPlot->getStartIndex() +
                    qRound(m_dataPlot->getNumberOfPossiblyDisplayedSamples()),
                    static_cast<int>(m_currentData.bytes().size()));
            m_dataPlot->setStartIndex(si);
            break;

        case QAbstractSlider::SliderPageStepSub:
            si = QMAX(0,  m_dataPlot->getStartIndex() -
                    qRound(m_dataPlot->getNumberOfPossiblyDisplayedSamples()));
            m_dataPlot->setStartIndex(si);
            break;

        default:
            break;
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateLeft() throw ()
{
    navigate(QAbstractSlider::SliderSingleStepSub);
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateRight() throw ()
{
    navigate(QAbstractSlider::SliderSingleStepAdd);
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateLeftPage() throw ()
{
    navigate(QAbstractSlider::SliderPageStepSub);
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateRightPage() throw ()
{
    navigate(QAbstractSlider::SliderPageStepAdd);
}


// -------------------------------------------------------------------------------------------------
void DataView::jumpToLeftMarker() throw ()
{
    if (m_dataPlot->getLeftMarker() >= 0)
        m_dataPlot->setStartIndex(QMAX(0, m_dataPlot->getLeftMarker()));
}


// -------------------------------------------------------------------------------------------------
void DataView::jumpToRightMarker() throw ()
{
    if (m_dataPlot->getRightMarker() >= 0) {
        // 3 instead of 2 because in end() size() is lastElementIndex + 1
        m_dataPlot->setStartIndex(QMAX(0, m_dataPlot->getRightMarker() -
                                       m_dataPlot->getNumberOfDisplayedSamples() + 3)  );
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::saveScreenshot() throw ()
{
    QString fileName = Q3FileDialog::getSaveFileName(
        QString::null, tr("PNG files (*.png)"),
        this, "", tr("Choose file to save"));
    if (fileName.isNull())
        return;

    QPixmap screenshot = m_dataPlot->getScreenshot();
    if (!screenshot.save(fileName, "PNG"))
        QMessageBox::warning(this,
            tr("TFLA-01"), tr("Current view could not be saved. Maybe you "
                "you don't have permissions to that location."),
                QMessageBox::Ok, QMessageBox::NoButton);
}


// -------------------------------------------------------------------------------------------------
void DataView::exportToCSV()
    throw ()
{
    QString fileName;
    bool diff;

    ExportDialog* ed = new ExportDialog(QDir::currentDirPath(), this);
    ed->setMode(Q3FileDialog::AnyFile);

    if (ed->exec() != QDialog::Accepted)
        return;

    fileName = ed->selectedFile();
    diff = ed->getDiffMode();

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    Q3TextStream stream(&file);
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

    // write header
    stream << "number;1;2;3;4;5;6;7;8\n";

    unsigned int size = m_currentData.bytes().size();
    for (unsigned int i = 0; i < size; ++i) {
        unsigned char byte = m_currentData.bytes()[i];

        // keep the event loop running
        if (i % 100 == 0)
            qApp->processEvents();

        // check for a state change
        if (diff && i != 0 && i != (size - 1) &&
                byte == m_currentData.bytes()[i-1] &&
                byte == m_currentData.bytes()[i+1])
            continue;

        stream << i << ';';
        for (int j = 0; j < NUMBER_OF_BITS_PER_BYTE; ++j) {
            stream << (int)(bit_is_set(byte, j) ? 1 : 0);
            if (j != NUMBER_OF_BITS_PER_BYTE - 1)
                stream << ';';
        }

        stream << '\n';
    }

    QApplication::restoreOverrideCursor();

    file.close();
}

// vim: set sw=4 ts=4 tw=100 et:
