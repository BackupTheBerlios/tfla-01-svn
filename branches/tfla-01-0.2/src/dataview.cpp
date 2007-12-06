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
#include <qframe.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qscrollbar.h>
#include <qfiledialog.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qcursor.h>

#include "settings.h"
#include "dataview.h"
#include "dataplot.h"
#include "tfla01.h"
#include "exportdialog.h"
#include "importdialog.h"
#include "VCD_symbol.h"
#include "VcdReader.h"
#include "io_vcl.h"

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
    m_scrollBar = new WheelScrollBar(Qt::Horizontal, this);
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
   
    m_currentData = data;
    m_currentData.GetSampleValue(0);
    m_scrollDivisor =  m_currentData.NumSamples()  == 0
                       ? 1
                       : m_currentData.NumSamples() / 1000;
                       
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
	if (m_dataPlot->getNumberOfDisplayedSamples() < (int) m_currentData.NumSamples())
	{
    	m_dataPlot->setZoomFactor(m_dataPlot->getZoomFactor() / 2.0);
	}
}


// -------------------------------------------------------------------------------------------------
void DataView::zoomFit()
    throw ()
{
    if (m_currentData.NumSamples() > 0)
    {
        m_dataPlot->setStartIndex(0);
        m_dataPlot->setZoomFactor( static_cast<double>(m_dataPlot->getCurrentWidthForPlot() - 1) / 
                              m_dataPlot->getPointsPerSample() / m_currentData.NumSamples() );
    }
    else
    {
        static_cast<Tfla01*>(qApp->mainWidget())->statusBar()->message(   
            tr("Function only available if data is gathered"), 4000); 
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
    if (m_dataPlot->getLeftMarker() > 0 && m_dataPlot->getRightMarker() > 0)
    {
        double diff = DABS(m_dataPlot->getRightMarker() - m_dataPlot->getLeftMarker());
        m_dataPlot->setZoomFactor( static_cast<double>(m_dataPlot->getCurrentWidthForPlot() - 2) / 
                              m_dataPlot->getPointsPerSample() / diff );
        m_dataPlot->setStartIndex(QMIN(m_dataPlot->getLeftMarker(), m_dataPlot->getRightMarker()));
    }
    else
    {
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
    if (m_currentData.NumSamples() > 0)
    {
        m_dataPlot->setStartIndex(m_currentData.NumSamples() - 
                                  m_dataPlot->getNumberOfPossiblyDisplayedSamples() + 2);
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::wheelEvent(QWheelEvent* e)
{
    switch (e->state())
    {
        case Qt::ControlButton:
            if (e->delta() > 0)
            {
                zoomIn();
            }
            else
            {
                zoomOut();
            }
            break;
            
        case Qt::ShiftButton:
            if (e->delta() > 0)
            {
                navigateLeftPage();
            }
            else
            {
                navigateRightPage();
            }
            break;
            
        case Qt::NoButton:
            if (e->delta() > 0)
            {
                navigateLeft();
            }
            else
            {
                navigateRight();
            }
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
    
    m_scrollBar->setRange(0, (m_currentData.NumSamples() - ps) / m_scrollDivisor);
    m_scrollBar->setValue(m_dataPlot->getStartIndex() / m_scrollDivisor);
    
    // set this to calculate the size
    if ((m_currentData.NumSamples() - ps) == 0)
    {
        m_scrollBar->setPageStep(0);
    }
    else
    {
        m_scrollBar->setPageStep(ps / m_scrollDivisor);
        m_scrollBar->setLineStep(qRound(ps / 10.0 / m_scrollDivisor)); 

#if 1
        PRINT_TRACE("div = %d", m_scrollDivisor);
        PRINT_TRACE2( "range = %d to %d", m_scrollBar->minValue(), m_scrollBar->maxValue());
        PRINT_TRACE("ps = %d", m_scrollBar->pageStep());
#endif
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
    m_dataPlot->setStartIndex( QMAX(0,  m_dataPlot->getStartIndex() -
                                        qRound(m_dataPlot->getNumberOfPossiblyDisplayedSamples()
                                               / 10.0)) );
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateRight() throw ()
{
    m_dataPlot->setStartIndex( QMIN(m_dataPlot->getStartIndex() +
                                   qRound(m_dataPlot->getNumberOfPossiblyDisplayedSamples() / 10.0),
                                   static_cast<int>(m_currentData.NumSamples())) );
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateLeftPage() throw ()
{
    int si = QMAX(0,  m_dataPlot->getStartIndex() -
                      qRound(m_dataPlot->getNumberOfPossiblyDisplayedSamples()));
    m_dataPlot->setStartIndex(si);
}


// -------------------------------------------------------------------------------------------------
void DataView::navigateRightPage() throw ()
{
    int si = QMIN( m_dataPlot->getStartIndex() +
                  qRound(m_dataPlot->getNumberOfPossiblyDisplayedSamples()),
                  static_cast<int>(m_currentData.NumSamples()));
    m_dataPlot->setStartIndex(si);
}


// -------------------------------------------------------------------------------------------------
void DataView::jumpToLeftMarker() throw ()
{
    if (m_dataPlot->getLeftMarker() >= 0)
    {
        m_dataPlot->setStartIndex(QMAX(0, m_dataPlot->getLeftMarker()));
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::jumpToRightMarker() throw ()
{
    if (m_dataPlot->getRightMarker() >= 0)
    {
        // 3 instead of 2 because in end() size() is lastElementIndex + 1
        m_dataPlot->setStartIndex(QMAX(0, m_dataPlot->getRightMarker() -
                                       m_dataPlot->getNumberOfDisplayedSamples() + 3)  );
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::saveScreenshot() throw ()
{
	Settings& set = Settings::set();
	QString BaseDir = set.readEntry("Measuring/ExportDir",QDir::currentDirPath());

    QString fileName = QFileDialog::getSaveFileName(
        BaseDir, tr("PNG files (*.png)"),
        this, "", tr("Choose file to save"));
    if (!fileName)
    {
        return;
    }
    
    QPixmap screenshot = m_dataPlot->getScreenshot();
    if (!screenshot.save(fileName, "PNG"))
    {
        QMessageBox::warning(this,
            tr("TFLA-01"), tr("Current view could not be saved. Maybe you "
                "you don't have permissions to that location."),
                QMessageBox::Ok, QMessageBox::NoButton);
    }
}


// -------------------------------------------------------------------------------------------------
void DataView::exportToVcd()  throw ()
{
    QString fileName;
    bool diff;
    unsigned i;
    unsigned start_sample;
    unsigned last_sample;
	Settings& set = Settings::set();
	QString BaseDir = set.readEntry("Measuring/ExportDir",QDir::currentDirPath());
    ExportDialog* ed = new ExportDialog(BaseDir, this);
    ed->setMode(QFileDialog::AnyFile);
  
    if (ed->exec() != QDialog::Accepted)
    {
        return;
    }
	set.writeEntry("Measuring/ExportDir",ed->dirPath());
    fileName = ed->selectedFile();
    diff = ed->getStateOption();

    QFile file(fileName);
    if (!file.open(IO_WriteOnly))
    {
        return;
    }

//    QTextStream stream(&file);
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

	io_vcl stream;
    unsigned int size = m_currentData.NumSamples();


	stream.num_wires = NUMBER_OF_WIRE_PER_SAPMPLE;
	stream.valid_wires = 0xFFFF;
	stream.SetTimeScale(size,m_currentData.getMeasuringTimeS());
    // write header
    if (ed->getCvsMode())
    {
		stream.Open(file,(diff)?1:0, m_currentData.m_StartTime);
    }
    else
    {
		stream.Open(file,2,m_currentData.m_StartTime);
    }
	if (ed->getCutMode())
	{
		int l=m_dataPlot->getLeftMarker();
		int p=m_dataPlot->getRightMarker();
		if (l>p)
		{
			// zamiana markerow
			start_sample = (p<1)?0:p;
			last_sample   = (l<1 || (unsigned)l>= size)?size:l+1;
		}
		else
		{
			start_sample = (l<1)?0:l;
			last_sample   = (p<1 || (unsigned)p>=size)?size:p+1;
		}
	}
	else
	{
		start_sample = 0;
		last_sample = size;
	}
    unsigned val_s = m_currentData.GetSampleValue(start_sample);
	stream.SendFirstSample(val_s);
	
    for (i = start_sample+1; i < last_sample-1; ++i)
    {
        val_s = m_currentData.GetSampleValue(i);
        stream.SendSample(val_s);
        // keep the event loop running
        if ((i & 0x3FF) == 0)
        {
            qApp->processEvents();
        }

    }
    val_s = m_currentData.GetSampleValue(i);
    stream.SendLastSample(val_s);
    stream.Close();
    QApplication::restoreOverrideCursor();
    file.close();
}

// -------------------------------------------------------------------------------------------------
void DataView::importFromVcd()  throw ()
{
    QString fileName;
	int result;
   	unsigned line_count;
	long 	num_records;
	unsigned nw,k;
	unsigned c_value,n_value;
	long	 c_time,n_time;
	
	Settings& set = Settings::set();
	QString BaseDir = set.readEntry("Measuring/ImportDir",QDir::currentDirPath());

    ImportDialog* ed = new ImportDialog(BaseDir, this);
    ed->setMode(QFileDialog::ExistingFile);
  
    if (ed->exec() != QDialog::Accepted)
    {
        return;
    }

    fileName = ed->selectedFile();
    QFile file(fileName);
    if (!file.open(IO_ReadOnly))
    {
        return;
    }
	set.writeEntry("Measuring/ImportDir",ed->dirPath());


	QTextStream stream( &file );
    QCString line;
	VcdReader src_f;

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

   	line_count =0;
   	num_records = -1;
    src_f.SetMode(ed->getCsvMode()?1:2);
    Data DataBuffer;
    DataBuffer.Open(100000);
    c_time = c_value =0;

	while ( !stream.atEnd() ) 
    {
    	line = stream.readLine(); // line of text excluding '\n'
      	src_f.InputLine(line);
  		line_count+=1;
  		do
  		{
  			result=src_f.ScanData();
	  		if (result == VCD_RC_EOF)
	  		{
	  			break;
	  		}
	  		switch(result)
	  		{
	  			case VCD_RC_NOP:
	  			break;
	  			
	  			case VCD_RC_DATA_START:
					num_records =0;
					c_time = 0;
					nw = (src_f.wire_num < NUMBER_OF_WIRE_PER_SAPMPLE) ?src_f.wire_num: NUMBER_OF_WIRE_PER_SAPMPLE;
	  			break;

	  			case VCD_RC_DATA:
					n_value = 0;
					for (k=0; k<nw; k+=1)
					{
						if (src_f.c_val[k])
						{
							n_value |= (1<<k);
						}
					}
					n_time = src_f.c_time;
	  				if (num_records)
	  				{
						DataBuffer.AddSample(c_value,n_time-c_time);
	  				}
					c_time =n_time;	  					
	  				c_value=n_value;
					c_time = src_f.c_time;
					num_records +=1;
	  			break;

	  			case VCD_RC_COMMENT:
		  		break;
		  		case VCD_RC_ERROR:
		  		break;
	  		}
  		} while (result != VCD_RC_NO_DATA);
  		
  		if (result == VCD_RC_EOF)
	  	{
	  		break;
	  	}
  		
   	}
    QApplication::restoreOverrideCursor();
	if (num_records >= 0)
	{
		if (src_f.EndOfData()==VCD_RC_DATA)
		{
			n_value = 0;
			for (k=0; k<nw; k+=1)
			{
				if (src_f.c_val[k])
				{
						n_value |= (1<<k);
				}
			}
			n_time = src_f.c_time;
			DataBuffer.AddSample(c_value,n_time-c_time);
		}
		DataBuffer.setSampleTime(src_f.GetSampleTime());
	    setData(DataBuffer);
	}
    file.close();
}

void DataView::resampleData()  throw ()
{
	if (m_currentData.NumSamples()<2)
	{
        static_cast<Tfla01*>(qApp->mainWidget())->statusBar()->message(   
            tr("Function only available if data is gathered"), 4000); 
		return;
	}
}

void DataView::trimsampleData()  throw ()
{
	if (m_currentData.NumSamples()<2)
	{
        static_cast<Tfla01*>(qApp->mainWidget())->statusBar()->message(   
            tr("Function only available if data is gathered"), 4000); 
		return;
	}
}

// vim: set sw=4 ts=4 et:
