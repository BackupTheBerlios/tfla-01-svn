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

#include <qobject.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qwidget.h>
#include <qprocess.h>
#include <qtextcodec.h>
#include <qfile.h>

#include "global.h"
#include "tfla01.h"
#include "help.h"
#include "settings.h"
#include "aboutdialog.h"

// -------------------------------------------------------------------------------------------------
void Help::showAbout()
{
    std::auto_ptr<AboutDialog> dlg(new AboutDialog(qApp->mainWidget(), "About Dialog"));
    
    dlg->exec();
}


// -------------------------------------------------------------------------------------------------
void Help::showHelp()
{
    QString base = QDir(qApp->applicationDirPath() + "/../share/tfla-01/doc/").canonicalPath();
    QString loc = QString(QTextCodec::locale()).section("_", 0, 0);
    
    if (QFile::exists(base + "/" + loc + "/index.html"))
    {
        openURL(qApp->mainWidget(), "file:///" + base + "/" + loc + "/index.html");
    }
    else if (QFile::exists(base + "/en/index.html"))
    {
        openURL(qApp->mainWidget(), "file:///" + base + "/en/index.html");
    }
    else
    {
        QMessageBox::critical(qApp->mainWidget(), tr("TFLA-01"), 
            tr("The TFLA-01 documentation is not installed."), QMessageBox::Ok, 
            QMessageBox::NoButton );
    }
}


// -------------------------------------------------------------------------------------------------
void Help::openURL(QWidget* parent, const QString& url)
{
    QString command = Settings::set().readEntry("General/Webbrowser");
    QProcess* process = new QProcess(command, parent);
    process->addArgument(url);
    
    if (! process->start())
    {
        QMessageBox::critical(parent, "TFLA-01",
            tr("<qt><p>Failed to open the link <tt>%1</tt> in the specified web browser."
                " The command was:</p><p><tt><nobr>%2</tt></nobr></p></qt>").arg(url).arg(
                command+" " + url), QMessageBox::Ok, QMessageBox::NoButton);
    }
}

