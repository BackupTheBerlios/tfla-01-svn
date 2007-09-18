/* 
 * Copyright (c) 2007, Bernhard Walle
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
#include <qhbox.h>

#include "exportdialog.h"

// -----------------------------------------------------------------------------
ExportDialog::ExportDialog(const QString &dirName, QWidget *parent)
    : QFileDialog(dirName, NULL, parent, "ExportDialog", true)
{
    QHBox *box = new QHBox(this);

    m_diffModeCB = new QCheckBox(tr("Save only &state changes (reduces file size)"), box);
    QWidget *filler = new QWidget(box);
    box->setStretchFactor(m_diffModeCB, 0);
    box->setStretchFactor(filler, 1);

    addWidgets(0, box, 0);

    setFilter(tr("CSV files (*.csv)"));
    setCaption(tr("Choose file name for exported data"));
}

// -----------------------------------------------------------------------------
bool ExportDialog::getDiffMode() const
{
    return m_diffModeCB->isChecked();
}

// vim: set sw=4 ts=4 et:
