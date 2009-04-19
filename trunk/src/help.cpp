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
#include <q3process.h>
#include <qtextcodec.h>
#include <qfile.h>

#include "global.h"
#include "tfla01.h"
#include "help.h"
#include "settings.h"
#include "aboutdialog.h"


// -------------------------------------------------------------------------------------------------
Help::Help() throw ()
{
    m_client = new QAssistantClient("", this);


}

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
    QStringList args;

    if (QFile::exists(base + "/" + loc + "/index.html"))
    {
        args += "-profile";
        args += QString(base + "/" + loc + "assistantconfig.xml");
        m_client->setArguments(args);
        m_client->openAssistant();
        m_client->showPage(base + "/" + loc + "/index.html");
    }
    else if (QFile::exists(base + "/en/index.html"))
    {
        args += "-profile";
        args += QString(base + "/en/assistantconfig.xml");
        m_client->setArguments(args);
        m_client->openAssistant();
        m_client->showPage(base + "/en/index.html");
    }
    else
    {
        QMessageBox::critical(qApp->mainWidget(), tr("TFLA-01"),
            tr("The TFLA-01 documentation is not installed."), QMessageBox::Ok,
            QMessageBox::NoButton );
    }
}

// vim: set sw=4 ts=4 tw=100:
