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
#include <qiconset.h>
#include <qapplication.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qcolordialog.h>

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
    setIcon(QPixmap::fromMimeSource("tfla-01_32.png"));
    setCaption(tr("The Fabulous Logic Analyzer"));
    
    setUsesBigPixmaps(true);
    QIconSet::setIconSize(QIconSet::Small, QSize(16, 16));
    QIconSet::setIconSize(QIconSet::Large, QSize(22, 22));
    
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
    sample_time_t nsa;
    
    m_actions.startAction->setEnabled(false);
    m_actions.stopAction->setEnabled(true);
    m_analyzingActive = true;
    
    Settings& set = Settings::set();
    
    auto_ptr<DataCollector> coll(new DataCollector(set.readNumEntry("Hardware/Parport")));
    coll->setCollectingTime(  (  set.readNumEntry("Measuring/Triggering/Minutes") * 60 + 
                                 set.readNumEntry("Measuring/Triggering/Seconds") ) * 1000  );
    coll->setTriggering(true,    set.readNumEntry("Measuring/Triggering/Value"), 
                                 set.readNumEntry("Measuring/Triggering/Mask") );
    coll->setNumberOfSkips(set.readNumEntry("Measuring/Number_Of_Skips"));
    
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    coll->start();
    
    // loop while the thread has finished
    QTimer timer;
    timer.start(100, 0);
    while (coll->running() && m_analyzingActive)
    {
        qApp->eventLoop()->processEvents(QEventLoop::WaitForMore);
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
    if (coll->getErrorString())
    {
        QMessageBox::warning(this, tr("TFLA-01"), 
            tr("<qt><nobr>An error occurred while collecting data:</nobr><br>%1</qt>").arg(
                coll->getErrorString()),
            QMessageBox::Ok, QMessageBox::NoButton);
        goto end;
    }
    nsa = coll->getData().getNumSamples();
    statusBar()->message(tr("Collected %1 samples successfully.").arg( 
                         loc.toString(  static_cast<double>(nsa), 
                                         'g', 
                                         QString::number(coll->getData().getNumSamples()).length() 
                                     )), 
                         4000);
    
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
void Tfla01::changeBrowser() throw ()
{
    bool ok;
    QString text = QInputDialog::getText( tr("TFLA-01"),
            tr("<qt>Enter the web browser <small>(must be in PATH environment "
                "or specify the full path)</small>:</qt>"),
            QLineEdit::Normal,
            Settings::set().readEntry("General/Webbrowser"), &ok, this);
    if (ok && !text.isEmpty()) 
    {
        Settings::set().writeEntry("General/Webbrowser", text);
        statusBar()->message(tr("Changed web browser successfully."), 4000);
    }
    else 
    {
        statusBar()->message(tr("Left web browser setting unchanged."), 4000);
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
    QPopupMenu* fileMenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&File"), fileMenu);

    m_actions.exportAction->addTo(fileMenu);
    m_actions.importAction->addTo(fileMenu);
    
    m_actions.saveViewAction->addTo(fileMenu);
    fileMenu->insertSeparator();
    m_actions.quitAction->addTo(fileMenu);
    
    // ----- Analyze -------------------------------------------------------------------------------
    QPopupMenu* analyzeMenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&Analyze"), analyzeMenu);
    m_actions.startAction->addTo(analyzeMenu);
    m_actions.stopAction->addTo(analyzeMenu);
    
    // ----- Edit ----------------------------------------------------------------------------------
    QPopupMenu* editMenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&Edit"), editMenu);

    m_actions.zoomInAction->addTo(editMenu);
    m_actions.zoomOutAction->addTo(editMenu);
    m_actions.resampleAction->addTo(editMenu);
    m_actions.trimsampleAction->addTo(editMenu);

    // ----- View ----------------------------------------------------------------------------------
    QPopupMenu* viewMenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&View"), viewMenu);

    m_actions.zoomInAction->addTo(viewMenu);
    m_actions.zoomOutAction->addTo(viewMenu);
    m_actions.zoom1Action->addTo(viewMenu);
    m_actions.zoomFitAction->addTo(viewMenu);
    m_actions.zoomMarkersAction->addTo(viewMenu);
    
    // ----- Navigate ------------------------------------------------------------------------------
    QPopupMenu* navigateMenu = new QPopupMenu(this);
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
    QPopupMenu* settingsMenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&Settings"), settingsMenu);
    
    m_actions.changeForegroundColorAction->addTo(settingsMenu);
    m_actions.changeLeftColorAction->addTo(settingsMenu);
    m_actions.changeRightColorAction->addTo(settingsMenu);
    //m_actions.changeBrowserAction->addTo(settingsMenu);
    
    m_portsMenu = new QPopupMenu(this);
    settingsMenu->insertItem(tr("&Port"), m_portsMenu);
    portChange(Settings::set().readNumEntry("Hardware/Parport"));
    
    // right align on Motif
    menuBar()->insertSeparator();
    
    // ----- Help ---------------------------------------------------------------------------------
    menuBar()->insertSeparator();
    QPopupMenu* helpMenu = new QPopupMenu(this);
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
    m_actions.exportAction = new QAction(QIconSet( QPixmap::fromMimeSource("stock_export_16.png"),
        QPixmap::fromMimeSource("stock_export_24.png") ), tr("&Export data..."),
        QKeySequence(CTRL|Key_E), this);

    m_actions.importAction = new QAction(QIconSet( QPixmap::fromMimeSource("stock_last_16.png"),
        QPixmap::fromMimeSource("stock_last_24.png") ), tr("&Read vcd file..."),
        QKeySequence(CTRL|Key_R), this);

    m_actions.saveViewAction = new QAction(QIconSet( QPixmap::fromMimeSource("stock_convert_16.png"),
        QPixmap::fromMimeSource("stock_convert_24.png") ), tr("&Save current plot..."),
        QKeySequence(CTRL|Key_S), this);

    m_actions.quitAction = new QAction(QIconSet( QPixmap::fromMimeSource("stock_exit_16.png"),
        QPixmap::fromMimeSource("stock_exit_24.png") ), tr("E&xit"),
        QKeySequence(CTRL|Key_Q), this);
    
    // ----- Edit ----------------------------------------------------------------------------------
    
    m_actions.resampleAction = new QAction(
        tr("&Re-sample Data"), QKeySequence(), this);
    
    m_actions.trimsampleAction = new QAction(
        tr("&Trim sample Data"), QKeySequence(), this);

//    m_actions.clearsampleAction = new QAction(tr("&Clear sample Data"), QKeySequence(), this);

    // ----- View ----------------------------------------------------------------------------------
    m_actions.zoomInAction = new QAction(QIconSet( QPixmap::fromMimeSource("stock_zoom_in_16.png"),
        QPixmap::fromMimeSource("stock_zoom_in_24.png") ), tr("Zoom &In"), 
        QKeySequence(Key_Plus), this);
    m_actions.zoomOutAction = new QAction(QIconSet( QPixmap::fromMimeSource("stock_zoom-out_16.png"),
        QPixmap::fromMimeSource("stock_zoom-out_24.png") ), tr("Zoom &Out"), 
        QKeySequence(Key_Minus), this);
    m_actions.zoomFitAction = new QAction(QIconSet( 
        QPixmap::fromMimeSource("stock_zoom-page-width_16.png"),
        QPixmap::fromMimeSource("stock_zoom-page-width_24.png") ), tr("Zoom &Fit"), 
        QKeySequence(Key_F3), this);
    m_actions.zoom1Action = new QAction(QIconSet( QPixmap::fromMimeSource("stock_zoom-1_16.png"),
        QPixmap::fromMimeSource("stock_zoom-1_24.png") ), tr("Zoom &Default"), 
        QKeySequence(Key_F2), this);
    m_actions.zoomMarkersAction = new QAction(QIconSet(
        QPixmap::fromMimeSource("stock_zoom-optimal_16.png"),
        QPixmap::fromMimeSource("stock_zoom-optimal_24.png")),
        tr("Zoom to fit &markers"),
        QKeySequence(Key_F4), this);
    
    // ----- Analyze -------------------------------------------------------------------------------
    m_actions.startAction = new QAction(QIconSet( QPixmap::fromMimeSource("stock_redo_16.png"),
        QPixmap::fromMimeSource("stock_redo_24.png") ), tr("&Start"), 
        QKeySequence(Key_F5), this);
    m_actions.stopAction = new QAction(QIconSet( QPixmap::fromMimeSource("stock_stop_16.png"),
        QPixmap::fromMimeSource("stock_stop_24.png") ), tr("&Start"), 
        QKeySequence(Key_F6), this);
    
    // ----- Navigate ------------------------------------------------------------------------------
    m_actions.navigatePos1Action = new QAction(QIconSet(
        QPixmap::fromMimeSource("stock_first_16.png"),
        QPixmap::fromMimeSource("stock_first_24.png")),
        tr("&Begin"), QKeySequence(Key_Home), this);
    m_actions.navigateEndAction = new QAction(QIconSet(
        QPixmap::fromMimeSource("stock_last_16.png"),
        QPixmap::fromMimeSource("stock_last_24.png")),
        tr("&End"), QKeySequence(Key_End), this);
    m_actions.navigatePageLeftAction = new QAction(QIconSet(
        QPixmap::fromMimeSource("stock_previous-page_16.png"),
        QPixmap::fromMimeSource("stock_previous-page_24.png")),
        tr("Page l&eft"), QKeySequence(Key_PageUp), this);
    m_actions.navigatePageRightAction = new QAction(QIconSet(
        QPixmap::fromMimeSource("stock_next-page_16.png"),
        QPixmap::fromMimeSource("stock_next-page_24.png")),
        tr("Page r&ight"), QKeySequence(Key_PageDown), this);
    m_actions.navigateLeftAction = new QAction(QIconSet(
        QPixmap::fromMimeSource("stock_left_arrow_16.png"),
        QPixmap::fromMimeSource("stock_left_arrow_24.png")),
        tr("&Left"), QKeySequence(Key_Left), this);
    m_actions.navigateRightAction = new QAction(QIconSet(
        QPixmap::fromMimeSource("stock_right_arrow_16.png"),
        QPixmap::fromMimeSource("stock_right_arrow_24.png")),
        tr("&Right"), QKeySequence(Key_Right), this);
    m_actions.jumpLeftAction = new QAction(
        tr("&Jump to left marker"), QKeySequence(CTRL|Key_Left), this);
    m_actions.jumpRightAction = new QAction(
        tr("J&ump to right marker"), QKeySequence(CTRL|Key_Right), this);
    
    // ----- Settings ------------------------------------------------------------------------------
    m_actions.changeForegroundColorAction = new QAction(QIconSet(
        QPixmap::fromMimeSource("stock_3d-color-picker_16.png"),
        QPixmap::fromMimeSource("stock_3d-color-picker_24.png")),
        tr("Change &foreground color..."), QKeySequence(), this);
    m_actions.changeLeftColorAction = new QAction(
        tr("Change color of &left marker..."), QKeySequence(), this);
    m_actions.changeRightColorAction = new QAction(
        tr("Change color of &right marker..."), QKeySequence(), this);
    m_actions.changeBrowserAction = new QAction(QIconSet(
        QPixmap::fromMimeSource("network_16.png"),
        QPixmap::fromMimeSource("network_24.png")),
        tr("Change &browser..."), QKeySequence(), this);
        
    // ----- Help ----------------------------------------------------------------------------------
    m_actions.helpAction = new QAction(QIconSet(QPixmap::fromMimeSource("stock_help_16.png"),
        QPixmap::fromMimeSource("stock_help_24.png")), tr("&Help"), QKeySequence(Key_F1), this);
    m_actions.aboutAction = new QAction(QIconSet(QPixmap::fromMimeSource("info_16.png"),
        QPixmap::fromMimeSource("info_24.png")), tr("&About..."), 0, this);
    m_actions.aboutQtAction = new QAction(QPixmap::fromMimeSource("qt_16.png"), 
        tr("About &Qt..."), QKeySequence(), this);
    
}


// -------------------------------------------------------------------------------------------------
void Tfla01::initToolbar()
    throw ()
{
    QToolBar* applicationToolbar = new QToolBar(this);
    applicationToolbar->setLabel(tr("Application"));
    
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
            m_centralWidget->getDataView(),        SLOT(exportToVcd()));

    connect(m_actions.importAction,                SIGNAL(activated()), 
            m_centralWidget->getDataView(),        SLOT(importFromVcd()));

    connect(m_actions.resampleAction,              SIGNAL(activated()), 
            m_centralWidget->getDataView(),        SLOT(resampleData()));

    connect(m_actions.trimsampleAction,            SIGNAL(activated()), 
            m_centralWidget->getDataView(),        SLOT(trimsampleData()));

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
    connect(m_actions.changeBrowserAction,         SIGNAL(activated()),
            this,                                  SLOT(changeBrowser()));
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

