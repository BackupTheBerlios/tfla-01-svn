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
#include <qlabel.h>
#include <q3hbox.h>
#include <q3vbox.h>
#include <qfile.h>
#include <q3textedit.h>
#include <qapplication.h>
#include <qdir.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3TextStream>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

#include "aboutdialog.h"

// -------------------------------------------------------------------------------------------------
AboutDialog::AboutDialog(QWidget* parent, const char* name)
    throw ()
    : QDialog(parent, name)
{
     m_mainLayout = new Q3VBoxLayout(this, 10, 6);
    setCaption(tr("About The Fabulous Logic Analyzer"));

    // the top of the dialog
    Q3HBox* titleBox = new Q3HBox(this);
    QLabel* titleIcon = new QLabel(titleBox);
    titleIcon->setPixmap(QPixmap(":/images/tfla-01_32.png"));
    QLabel* titleText = new QLabel(tr("<p><b>The Fabulous Logic Analyzer "VERSION_STRING"</b></p>"),
            titleBox);
    titleBox->setStretchFactor(titleIcon, 0);
    titleBox->setStretchFactor(titleText, 5);
    titleBox->setSpacing(5);

    // the tab
    m_tabWidget = new QTabWidget(this);
    setupAboutTab();
    setupLicenseTab();

    // the ok Button
    QPushButton* okButton = new QPushButton(tr("&Close"), this, "Close button");
    okButton->setDefault(true);
    QWidget* filler = new QWidget(this);
    Q3HBoxLayout* buttonLayout = new Q3HBoxLayout(0, 0, 6);
    buttonLayout->addWidget(filler);
    buttonLayout->addWidget(okButton);
    buttonLayout->setStretchFactor(filler, 1);
    buttonLayout->setStretchFactor(okButton, 0);

    // main layout
    m_mainLayout->addWidget(titleBox);
    m_mainLayout->addWidget(m_tabWidget);
    m_mainLayout->addStretch(5);
    m_mainLayout->addLayout(buttonLayout);

    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
}


// -------------------------------------------------------------------------------------------------
void AboutDialog::setupAboutTab()
{
    Q3HBox* aboutTab = new Q3HBox(this);
    aboutTab->setMargin(15);

    (void)new QLabel(
        tr("<p><nobr>This is a logic analyzer tool written for Unix and Windows"
            "</nobr> written in C++ using the Qt library.</p>"
            "<p><b>Thanks to:</b>"
            "<ul><li>Trolltech for the Qt library</li>"
            "<li>Tim Waugh for the IEEE1284 (parallelport) library</li>"
            "<li>Gtk+ artists for the nice stock icons</li>"
            "</ul></p>"
            "<p><b>Homepage:</b> <tt>http://tfla-01.berlios.de</tt></p>"), aboutTab);

    m_tabWidget->addTab(aboutTab, tr("&About"));

}


// -------------------------------------------------------------------------------------------------
void AboutDialog::setupLicenseTab()
{
    Q3VBox* licenseTab = new Q3VBox(this);

    Q3TextEdit* textEdit = new Q3TextEdit(licenseTab);
    textEdit->setReadOnly(true);
    textEdit->setWordWrap(Q3TextEdit::FixedColumnWidth);
    textEdit->setWrapColumnOrWidth(100);

    QString fileName = qApp->applicationDirPath() + "/../share/tfla-01/COPYING";
    if (QFile::exists(fileName))
    {
        QFile file(fileName);
        if (file.open( QIODevice::ReadOnly ))
        {
            Q3TextStream stream(&file);
            textEdit->setText("<pre>" + stream.read() + "</pre>");
        }
    }

    m_tabWidget->addTab(licenseTab, tr("&License"));
}

// vim: set sw=4 ts=4 tw=100:
