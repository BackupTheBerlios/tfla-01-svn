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
#ifndef TFLA01_H
#define TFLA01_H

#include <q3mainwindow.h>
#include <qaction.h>
//Added by qt3to4:
#include <Q3PopupMenu>
#include <QCloseEvent>

#include "centralwidget.h"
#include "help.h"

class Tfla01 : public Q3MainWindow
{
    Q_OBJECT

    public:
        Tfla01() throw ();
        virtual ~Tfla01() {}

    protected:
        void initActions() throw ();
        void initMenubar() throw ();
        void initToolbar() throw ();
        void connectSignalsAndSlots() throw ();
        void closeEvent(QCloseEvent* evt);

    protected slots:
        void portChange(int id) throw ();
        void startAnalyze() throw ();
        void stopAnalyze() throw ();
        void changeForegroundColor() throw ();
        void changeLeftColor() throw ();
        void changeRightColor() throw ();

    private:
        struct
        {
            QAction* saveViewAction;
	    QAction* exportAction;
            QAction* quitAction;
            QAction* whatsThisAction;
            QAction* helpAction;
            QAction* aboutAction;
            QAction* aboutQtAction;
            QAction* zoomInAction;
            QAction* zoomOutAction;
            QAction* zoomFitAction;
            QAction* zoom1Action;
            QAction* zoomMarkersAction;
            QAction* startAction;
            QAction* stopAction;
            QAction* changeForegroundColorAction;
            QAction* changeLeftColorAction;
            QAction* changeRightColorAction;
            QAction* navigatePos1Action;
            QAction* navigateEndAction;
            QAction* navigateLeftAction;
            QAction* navigateRightAction;
            QAction* navigatePageLeftAction;
            QAction* navigatePageRightAction;
            QAction* jumpLeftAction;
            QAction* jumpRightAction;
        }
        m_actions;
        CentralWidget* m_centralWidget;
        Q3PopupMenu*    m_portsMenu;
        Help           m_help;
        bool           m_analyzingActive;

    private:
        Tfla01(const Tfla01&);
        Tfla01& operator=(const Tfla01&);
};

#endif /* TFLA01_H */

// vim: set sw=4 ts=4 tw=100:
