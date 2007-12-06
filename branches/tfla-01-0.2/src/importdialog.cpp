/* 
 * Copyright (c) 2007, Andrzej Kass
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

#include "importdialog.h"

// -----------------------------------------------------------------------------
ImportDialog::ImportDialog(const QString &dirName, QWidget *parent)
    : QFileDialog(dirName, NULL, parent, "ImportDialog", true)
{
    QHBox *vbox = new QHBox(this);


    m_CsvModeCB = new QCheckBox(tr("CSV input format, separator=';' "), vbox);
    QWidget *f2 = new QWidget(vbox);
    vbox->setStretchFactor(m_CsvModeCB, 0);
    vbox->setStretchFactor(f2, 1);

    addWidgets(0, vbox, 0);

    setFilter(tr("CSV files (*.csv)") );
	addFilter(tr("VCD files (*.vcd)"));
    setCaption(tr("Choose file name and format for Import data"));
}


// -----------------------------------------------------------------------------
bool ImportDialog::getCsvMode() const
{
    return m_CsvModeCB->isChecked();
}

