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
 /*
  * changelog
  * A.Kass add extra option for select VCD or CSV format
  *        CSV format have a variant with round(TimeStep) and State change only
  */
  
#include <qhbox.h>

#include "exportdialog.h"

// -----------------------------------------------------------------------------
ExportDialog::ExportDialog(const QString &dirName, QWidget *parent)
    : QFileDialog(dirName, NULL, parent, "ExportDialog", true)
{
    QHBox *box1 = new QHBox(this);
    QHBox *box2 = new QHBox(this);

    m_cutModeCB = new QCheckBox(tr("Market area only "), box1);
    QWidget *f1 = new QWidget(box1);
    box1->setStretchFactor(m_cutModeCB, 0);
    box1->setStretchFactor(f1, 5);

    m_cvsModeCB  = new QCheckBox(tr("CVS output format "), box2);
    m_StateOptCB = new QCheckBox(tr("Use numbering Change of State "), box2);
    box2->setStretchFactor(m_cvsModeCB, 1);
    box2->setStretchFactor(m_StateOptCB, 3);

    addWidgets(0,box1, 0);  /* Area betwen makres */
    addWidgets(0,box2, 0);	/* CSV Options        */
    setFilter(tr("CSV files (*.csv)"));
	addFilter(tr("VCD files (*.vcd)"));
	connect(m_cvsModeCB,SIGNAL(clicked()),this,SLOT(clickedCvsMode()));
    setCaption(tr("Choose file name and format for exported data"));
}


// -----------------------------------------------------------------------------
bool ExportDialog::getStateOption() const
{
    return m_StateOptCB->isChecked();
}

// -----------------------------------------------------------------------------
bool ExportDialog::getCvsMode() const
{
    return m_cvsModeCB->isChecked();
}

// -----------------------------------------------------------------------------
bool ExportDialog::getCutMode() const
{
    return m_cutModeCB->isChecked();
}

void ExportDialog::clickedCvsMode()
{
    if (getCvsMode())
	{
	    setFilter(tr("CSV files (*.csv)") );
	}
	else
	{
	    setFilter(tr("VCD files (*.vcd)") );
	}
}


// vim: set sw=4 ts=4 et:
