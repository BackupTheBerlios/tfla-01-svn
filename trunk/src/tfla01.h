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

#include <qmainwindow.h>
#include <qaction.h>

#include "centralwidget.h"
#include "help.h"

class Tfla01 : public QMainWindow
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
            QAction* updateOnScrollAction;
            QAction* navigatePos1Action;
            QAction* navigateEndAction;
            QAction* navigateLeftAction;
            QAction* navigateRightAction;
            QAction* jumpLeftAction;
            QAction* jumpRightAction;
        }
        m_actions;
        CentralWidget* m_centralWidget;
        QPopupMenu*    m_portsMenu;
        Help           m_help;
        bool           m_analyzingActive;
        
    private:
        Tfla01(const Tfla01&);
        Tfla01& operator=(const Tfla01&);
};


#endif /* TFLA01_H */
