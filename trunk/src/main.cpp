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

#include <qstring.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qtranslator.h>
#include <qmessagebox.h>
#include <qtextcodec.h>

#include "hardware/parport.h"
#include "hardware/parportlist.h"
#include "tferror.h"
#include "tfla01.h"
#include "datacollector.h"

// -------------------------------------------------------------------------------------------------
using std::auto_ptr;

// -------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    // translation
    QTranslator translator(0), ttranslator(0);
    QString loc = QTextCodec::locale();
    translator.load(loc, qApp->applicationDirPath() + "/../share/tfla-01/translations/");
    ttranslator.load(QString("qt_") + loc, QString(getenv("QTDIR")) + "/translations/");
    app.installTranslator(&translator);
    app.installTranslator(&ttranslator);
 
    
    try
    {
        auto_ptr<Tfla01> tfla01(new Tfla01);
        
        app.setMainWidget(tfla01.get());
        
        tfla01->show();
        app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
        
        return app.exec();
    }
    catch (const TfError& e)
    {
        QMessageBox::warning(0, QObject::tr("TFLA-01"),
            QObject::tr("An unknown error occurred:\n%1\nThe application will be closed.")
            .arg(e.what()),
            QMessageBox::Ok | QMessageBox::Default, QMessageBox::NoButton);
    }
    catch (const std::bad_alloc& e)
    {
        QMessageBox::warning(0, QObject::tr("TFLA-01"), 
            QObject::tr("No more memory available. The application\nwill be closed."),
            QMessageBox::Ok | QMessageBox::Default, QMessageBox::NoButton); 
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(0, QObject::tr("TFLA-01"),
            QObject::tr("An unknown error occurred:\n%1\nThe application will be closed.")
            .arg(e.what()),
            QMessageBox::Ok | QMessageBox::Default, QMessageBox::NoButton);
    }
    
    return 1;
}
