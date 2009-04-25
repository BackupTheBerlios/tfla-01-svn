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
#include <memory>

#include <qlocale.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qinputdialog.h>
#include <qeventloop.h>
#include <qcursor.h>
#include <qicon.h>
#include <qapplication.h>
#include <q3popupmenu.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qcolordialog.h>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QToolBar>

#include "hardware/parportlist.h"
#include "tfla01.h"
#include "settings.h"
#include "datacollector.h"
#include "aboutdialog.h"

using std::auto_ptr;

// -------------------------------------------------------------------------------------------------
Tfla01::Tfla01()
    throw ()
    : QMainWindow(0, "tfla01 main window")
{
    // Title and Icon
    setIcon(QPixmap(":/images/tfla-01_32.png"));
    setCaption(tr("The Fabulous Logic Analyzer"));

    setIconSize(QSize(24, 24));

    // main widget in the center TODO
    m_centralWidget = new CentralWidget(this);
    setCentralWidget(m_centralWidget);

    // Initiamlization of menu
    initActions();
    initMenubar();
    initToolbar();

    // restore the layout
    if (Settings::set().readBoolEntry("Main Window/maximized"))
    {
        showMaximized();
    }
    else
    {
        resize(
            Settings::set().readNumEntry("Main Window/width", int(qApp->desktop()->width() * 0.6) ),
            Settings::set().readNumEntry("Main Window/height", int(qApp->desktop()->height() / 2.0) )
        );
    }

    // disable some actions
    m_actions.stopAction->setEnabled(false);

    connectSignalsAndSlots();
    statusBar()->setSizeGripEnabled(true);
}


// -------------------------------------------------------------------------------------------------
void Tfla01::portChange(int id) throw ()
{
    Settings::set().writeEntry("Hardware/Parport", id);
    m_portsMenu->clear();

    ParportList* list = ParportList::instance(0);
    for (int i = 0; i < list->getNumberOfPorts(); i++)
    {
        m_portsMenu->insertItem(list->getPort(i).getName(), i);
        m_portsMenu->setItemChecked(i, i == Settings::set().readNumEntry("Hardware/Parport"));
    }
}


// -------------------------------------------------------------------------------------------------
void Tfla01::startAnalyze()
    throw ()
{
    QLocale loc;

    m_actions.startAction->setEnabled(false);
    m_actions.stopAction->setEnabled(true);
    m_analyzingActive = true;

    Settings& set = Settings::set();

    auto_ptr<DataCollector> coll(new DataCollector(set.readNumEntry("Hardware/Parport")));
    coll->setCollectingTime(  (  set.readNumEntry("Measuring/Triggering/Minutes") * 60 +
                                 set.readNumEntry("Measuring/Triggering/Seconds") ) * 1000  );
    coll->setTriggering(true, set.readNumEntry("Measuring/Triggering/Value"),
                              set.readNumEntry("Measuring/Triggering/Mask") );
    coll->setNumberOfSkips(set.readNumEntry("Measuring/Number_Of_Skips"));

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    coll->start();

    // loop while the thread has finished
    QTimer timer;
    timer.start(100, 0);
    while (coll->running() && m_analyzingActive)
    {
        qApp->processEvents( QEventLoop::WaitForMore);
    }
    timer.stop();

    // stopped by the user
    if (!m_analyzingActive)
    {
        coll->stop();
        coll->wait();
    }
    QApplication::restoreOverrideCursor();

    // finished
    if (!coll->getErrorString().isNull())
    {
        QMessageBox::warning(this, tr("TFLA-01"),
            tr("<qt><nobr>An error occurred while collecting data:</nobr><br>%1</qt>").arg(
                coll->getErrorString()),
            QMessageBox::Ok, QMessageBox::NoButton);
        goto end;
    }

    statusBar()->message(tr("Collected %1 samples successfully.").arg(
                         loc.toString(  static_cast<double>(coll->getData().bytes().size()),
                                         'g',
                                         QString::number(coll->getData().bytes().size()).length()
                                     )),
                         2000);

    m_centralWidget->getDataView()->setData(coll->getData());

end:
    m_actions.startAction->setEnabled(true);
    m_actions.stopAction->setEnabled(false);
}


// -------------------------------------------------------------------------------------------------
void Tfla01::stopAnalyze() throw ()
{
    m_analyzingActive = false;
}


// -------------------------------------------------------------------------------------------------
void Tfla01::changeForegroundColor()  throw ()
{
    QString currentColor = Settings::set().readEntry("UI/Foreground_Color_Line");
    QColor col = QColorDialog::getColor(currentColor, this);

    if (col.isValid())
    {
        Settings::set().writeEntry("UI/Foreground_Color_Line", col.name());
        m_centralWidget->getDataView()->redrawData();
    }
}


// -------------------------------------------------------------------------------------------------
void Tfla01::changeLeftColor()
    throw ()
{
    QString currentColor = Settings::set().readEntry("UI/Left_Marker_Color");
    QColor col = QColorDialog::getColor(currentColor, this);

    if (col.isValid())
    {
        Settings::set().writeEntry("UI/Left_Marker_Color", col.name());
        m_centralWidget->getDataView()->redrawData();
    }
}


// -------------------------------------------------------------------------------------------------
void Tfla01::changeRightColor()  throw ()
{
    QString currentColor = Settings::set().readEntry("UI/Right_Marker_Color");
    QColor col = QColorDialog::getColor(currentColor, this);

    if (col.isValid())
    {
        Settings::set().writeEntry("UI/Right_Marker_Color", col.name());
        m_centralWidget->getDataView()->redrawData();
    }
}


// -------------------------------------------------------------------------------------------------
void Tfla01::closeEvent(QCloseEvent* e)
{
    // write window layout
    Settings::set().writeEntry("Main Window/width", size().width());
    Settings::set().writeEntry("Main Window/height", size().height());
    Settings::set().writeEntry("Main Window/maximized", isMaximized());

    e->accept();
}


// -------------------------------------------------------------------------------------------------
void Tfla01::initMenubar() throw ()
{
    // ----- File ----------------------------------------------------------------------------------
    Q3PopupMenu* fileMenu = new Q3PopupMenu(this);
    menuBar()->insertItem(tr("&File"), fileMenu);

    m_actions.exportAction->addTo(fileMenu);
    m_actions.saveViewAction->addTo(fileMenu);
    fileMenu->insertSeparator();
    m_actions.quitAction->addTo(fileMenu);

    // ----- Analyze -------------------------------------------------------------------------------
    Q3PopupMenu* analyzeMenu = new Q3PopupMenu(this);
    menuBar()->insertItem(tr("&Analyze"), analyzeMenu);
    m_actions.startAction->addTo(analyzeMenu);
    m_actions.stopAction->addTo(analyzeMenu);

    // ----- View ----------------------------------------------------------------------------------
    Q3PopupMenu* viewMenu = new Q3PopupMenu(this);
    menuBar()->insertItem(tr("&View"), viewMenu);

    m_actions.zoomInAction->addTo(viewMenu);
    m_actions.zoomOutAction->addTo(viewMenu);
    m_actions.zoom1Action->addTo(viewMenu);
    m_actions.zoomFitAction->addTo(viewMenu);
    m_actions.zoomMarkersAction->addTo(viewMenu);

    // ----- Navigate ------------------------------------------------------------------------------
    Q3PopupMenu* navigateMenu = new Q3PopupMenu(this);
    menuBar()->insertItem(tr("&Navigate"), navigateMenu);

    m_actions.navigatePos1Action->addTo(navigateMenu);
    m_actions.navigateEndAction->addTo(navigateMenu);
    m_actions.navigatePageLeftAction->addTo(navigateMenu);
    m_actions.navigatePageRightAction->addTo(navigateMenu);
    m_actions.navigateLeftAction->addTo(navigateMenu);
    m_actions.navigateRightAction->addTo(navigateMenu);
    navigateMenu->insertSeparator();
    m_actions.jumpLeftAction->addTo(navigateMenu);
    m_actions.jumpRightAction->addTo(navigateMenu);

    // ----- Settings---------------------------------------------------------------------------------
    Q3PopupMenu* settingsMenu = new Q3PopupMenu(this);
    menuBar()->insertItem(tr("&Settings"), settingsMenu);

    m_actions.changeForegroundColorAction->addTo(settingsMenu);
    m_actions.changeLeftColorAction->addTo(settingsMenu);
    m_actions.changeRightColorAction->addTo(settingsMenu);

    m_portsMenu = new Q3PopupMenu(this);
    settingsMenu->insertItem(tr("&Port"), m_portsMenu);
    portChange(Settings::set().readNumEntry("Hardware/Parport"));

    // right align on Motif
    menuBar()->insertSeparator();

    // ----- Help ---------------------------------------------------------------------------------
    menuBar()->insertSeparator();
    Q3PopupMenu* helpMenu = new Q3PopupMenu(this);
    menuBar()->insertItem(tr("&Help"), helpMenu);

    m_actions.helpAction->addTo(helpMenu);
    helpMenu->insertSeparator();
    m_actions.aboutQtAction->addTo(helpMenu);
    m_actions.aboutAction->addTo(helpMenu);
}


// -------------------------------------------------------------------------------------------------
void Tfla01::initActions()
    throw ()
{
    // ----- File ----------------------------------------------------------------------------------
	QIcon exportIcon(QPixmap(":/images/stock_export_16.png"));
	exportIcon.addPixmap(QPixmap(":/images/stock_export_24.png"));
    m_actions.exportAction = new QAction(exportIcon, tr("&Export data..."), this);
	m_actions.exportAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_E));

	QIcon saveViewIcon(QPixmap(":/images/stock_convert_16.png"));
	saveViewIcon.addPixmap(QPixmap(":/images/stock_convert_24.png"));
    m_actions.saveViewAction = new QAction(saveViewIcon, tr("&Save current plot..."), this);
	m_actions.exportAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_S));

	QIcon quitIcon(QPixmap(":/images/stock_exit_16.png"));
	quitIcon.addPixmap(QPixmap(":/images/stock_exit_24.png"));
    m_actions.quitAction = new QAction(quitIcon, tr("E&xit"), this);
	m_actions.quitAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_Q));

    // ----- View ----------------------------------------------------------------------------------
	QIcon zoomInIcon(QPixmap(":/images/stock_zoom_in_16.png"));
	zoomInIcon.addPixmap(QPixmap(":/images/stock_zoom_in_24.png"));
    m_actions.zoomInAction = new QAction(zoomInIcon, tr("Zoom &In"), this);
	m_actions.zoomInAction->setShortcut(QKeySequence(Qt::Key_Plus));

	QIcon zoomOutIcon(QPixmap(":/images/stock_zoom-out_16.png"));
	zoomOutIcon.addPixmap(QPixmap(":/images/stock_zoom-out_24.png"));
    m_actions.zoomOutAction = new QAction(zoomOutIcon, tr("Zoom &Out"), this);
	m_actions.zoomOutAction->setShortcut(QKeySequence(Qt::Key_Minus));

	QIcon zoomFitIcon(QPixmap(":/images/stock_zoom-page-width_16.png"));
	zoomFitIcon.addPixmap(QPixmap(":/images/stock_zoom-page-width_24.png"));
    m_actions.zoomFitAction = new QAction(zoomFitIcon, tr("Zoom &Fit"), this);
    m_actions.zoomFitAction->setShortcut(QKeySequence(Qt::Key_F3));

    QIcon zoom1Icon(QPixmap(":/images/stock_zoom-1_16.png"));
	zoom1Icon.addPixmap(QPixmap(":/images/stock_zoom-1_24.png"));
	m_actions.zoom1Action = new QAction(zoom1Icon, tr("Zoom &Default"), this);
	m_actions.zoom1Action->setShortcut(QKeySequence(Qt::Key_F2));

	QIcon zoomMarkersIcon(QPixmap(":/images/stock_zoom-optimal_16.png"));
	zoomMarkersIcon.addPixmap(QPixmap(":/images/stock_zoom-optimal_24.png"));
    m_actions.zoomMarkersAction = new QAction(zoomMarkersIcon, tr("Zoom to fit &markers"), this);
	m_actions.zoomMarkersAction->setShortcut(QKeySequence(Qt::Key_F4));

    // ----- Analyze -------------------------------------------------------------------------------
    QIcon startIcon(QPixmap(":/images/stock_redo_16.png"));
	startIcon.addPixmap(QPixmap(":/images/stock_redo_24.png"));
	m_actions.startAction = new QAction(startIcon, tr("&Start"), this);
	m_actions.startAction->setShortcut(QKeySequence(Qt::Key_F5));

	QIcon stopIcon(QPixmap(":/images/stock_stop_16.png"));
	stopIcon.addPixmap(QPixmap(":/images/stock_stop_24.png"));
    m_actions.stopAction = new QAction(stopIcon, tr("&Stop"), this);
	m_actions.stopAction->setShortcut(QKeySequence(Qt::Key_F6));

    // ----- Navigate ------------------------------------------------------------------------------
	QIcon navigatePos1Icon(QPixmap(":/images/stock_first_16.png"));
	navigatePos1Icon.addPixmap(QPixmap(":/images/stock_first_24.png"));
    m_actions.navigatePos1Action = new QAction(navigatePos1Icon, tr("&Begin"), this);
	m_actions.navigatePos1Action->setShortcut(QKeySequence(Qt::Key_Home));

	QIcon navigateEndIcon(QPixmap(":/images/stock_last_16.png"));
	navigateEndIcon.addPixmap(QPixmap(":/images/stock_last_24.png"));
    m_actions.navigateEndAction = new QAction(navigateEndIcon, tr("&End"), this);
	m_actions.navigateEndAction->setShortcut(QKeySequence(Qt::Key_End));

	QIcon navigatePageLeftIcon(QPixmap(":/images/stock_previous-page_16.png"));
	navigatePageLeftIcon.addPixmap(QPixmap(":/images/stock_previous-page_24.png"));
    m_actions.navigatePageLeftAction = new QAction(navigatePageLeftIcon, tr("Page l&eft"), this);
	m_actions.navigatePageLeftAction->setShortcut(QKeySequence(Qt::Key_PageUp));

	QIcon navigatePageRightIcon(QPixmap(":/images/stock_next-page_16.png"));
	navigatePageRightIcon.addPixmap(QPixmap(":/images/stock_next-page_24.png"));
    m_actions.navigatePageRightAction = new QAction(navigatePageRightIcon,
        tr("Page r&ight"), this);
	m_actions.navigatePageRightAction->setShortcut(QKeySequence(Qt::Key_PageDown));

	QIcon navigateLeftIcon(QPixmap(":/images/stock_left_arrow_16.png"));
	navigateLeftIcon.addPixmap(QPixmap(":/images/stock_left_arrow_24.png"));
    m_actions.navigateLeftAction = new QAction(navigateLeftIcon, tr("&Left"), this);
	m_actions.navigateLeftAction->setShortcut(QKeySequence(Qt::Key_Left));

	QIcon navigateRightIcon(QPixmap(":/images/stock_right_arrow_16.png"));
	navigateRightIcon.addPixmap(QPixmap(":/images/stock_right_arrow_24.png"));
    m_actions.navigateRightAction = new QAction(navigateRightIcon, tr("&Right"), this); 
	m_actions.navigateRightAction->setShortcut(QKeySequence(Qt::Key_Right));

    m_actions.jumpLeftAction = new QAction(tr("&Jump to left marker"), this);
	m_actions.jumpLeftAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_Left));

    m_actions.jumpRightAction = new QAction(tr("J&ump to right marker"), this);
	m_actions.jumpRightAction->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_Right));

    // ----- Settings ------------------------------------------------------------------------------
	QIcon changeForegroundColorIcon(QPixmap(":/images/stock_3d-color-picker_16.png"));
	changeForegroundColorIcon.addPixmap(QPixmap(":/images/stock_3d-color-picker_24.png"));
    m_actions.changeForegroundColorAction = new QAction(changeForegroundColorIcon,
        tr("Change &foreground color..."), this);

    m_actions.changeLeftColorAction = new QAction(tr("Change color of &left marker..."), this);

    m_actions.changeRightColorAction = new QAction(tr("Change color of &right marker..."), this);

    // ----- Help ----------------------------------------------------------------------------------
	QIcon helpIcon(QPixmap(":/images/stock_help_16.png"));
	helpIcon.addPixmap(QPixmap(":/images/stock_help_24.png"));
    m_actions.helpAction = new QAction(helpIcon, tr("&Help"), this);
	m_actions.helpAction->setShortcut(QKeySequence(Qt::Key_F1)); 

    QIcon aboutIcon(QPixmap(":/images/info_16.png"));
	aboutIcon.addPixmap(QPixmap(":/images/info_24.png"));
	m_actions.aboutAction = new QAction(aboutIcon, tr("&About..."), this);

	QIcon qtIcon(QPixmap(":/images/qt_16.png"));
    m_actions.aboutQtAction = new QAction(qtIcon, tr("About &Qt..."), this);

}


// -------------------------------------------------------------------------------------------------
void Tfla01::initToolbar()
    throw ()
{
    QToolBar* applicationToolbar = new QToolBar(this);
    applicationToolbar->setLabel(tr("Application"));
    addToolBar(applicationToolbar);

    m_actions.quitAction->addTo(applicationToolbar);
    applicationToolbar->addSeparator();
    m_actions.startAction->addTo(applicationToolbar);
    m_actions.stopAction->addTo(applicationToolbar);
    applicationToolbar->addSeparator();
    m_actions.zoomInAction->addTo(applicationToolbar);
    m_actions.zoomOutAction->addTo(applicationToolbar);
    m_actions.zoom1Action->addTo(applicationToolbar);
    m_actions.zoomFitAction->addTo(applicationToolbar);
    m_actions.zoomMarkersAction->addTo(applicationToolbar);
    applicationToolbar->addSeparator();
    m_actions.navigatePos1Action->addTo(applicationToolbar);
    m_actions.navigateEndAction->addTo(applicationToolbar);
    m_actions.navigatePageLeftAction->addTo(applicationToolbar);
    m_actions.navigatePageRightAction->addTo(applicationToolbar);
    m_actions.navigateLeftAction->addTo(applicationToolbar);
    m_actions.navigateRightAction->addTo(applicationToolbar);
    applicationToolbar->addSeparator();
    m_actions.helpAction->addTo(applicationToolbar);
}


// -------------------------------------------------------------------------------------------------
void Tfla01::connectSignalsAndSlots()
    throw ()
{
    connect(m_actions.saveViewAction,              SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(saveScreenshot()));
    connect(m_actions.exportAction,                SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(exportToCSV()));
    connect(m_actions.quitAction,                  SIGNAL(activated()),
            this,                                  SLOT(close()));
    connect(m_actions.helpAction,                  SIGNAL(activated()),
            &m_help,                               SLOT(showHelp()));
    connect(m_actions.aboutAction,                 SIGNAL(activated()) ,
            &m_help,                               SLOT(showAbout()));
    connect(m_actions.aboutQtAction,               SIGNAL(activated()),
            qApp,                                  SLOT(aboutQt()));
    connect(m_portsMenu,                           SIGNAL(activated(int)),
            this,                                  SLOT(portChange(int)));
    connect(m_actions.startAction,                 SIGNAL(activated()),
            this,                                  SLOT(startAnalyze()));
    connect(m_actions.stopAction,                  SIGNAL(activated()),
            this,                                  SLOT(stopAnalyze()));
    connect(m_actions.zoom1Action,                 SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(zoom1()) );
    connect(m_actions.zoomInAction,                SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(zoomIn()) );
    connect(m_actions.zoomOutAction,               SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(zoomOut()) );
    connect(m_actions.zoomFitAction,               SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(zoomFit()) );
    connect(m_actions.zoomMarkersAction,           SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(zoomMarkers()));
    connect(m_actions.changeForegroundColorAction, SIGNAL(activated()),
            this,                                  SLOT(changeForegroundColor()));
    connect(m_actions.changeLeftColorAction,       SIGNAL(activated()),
            this,                                  SLOT(changeLeftColor()));
    connect(m_actions.changeRightColorAction,      SIGNAL(activated()),
            this,                                  SLOT(changeRightColor()));
    connect(m_actions.navigatePos1Action,          SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(pos1()));
    connect(m_actions.navigateEndAction,           SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(end()));
    connect(m_actions.navigateLeftAction,          SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(navigateLeft()));
    connect(m_actions.navigateRightAction,         SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(navigateRight()));
    connect(m_actions.navigatePageLeftAction,      SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(navigateLeftPage()));
    connect(m_actions.navigatePageRightAction,     SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(navigateRightPage()));
    connect(m_actions.jumpLeftAction,              SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(jumpToLeftMarker()));
    connect(m_actions.jumpRightAction,             SIGNAL(activated()),
            m_centralWidget->getDataView(),        SLOT(jumpToRightMarker()));
}

// vim: set sw=4 ts=4 tw=100:
