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
#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <qfiledialog.h>
#include <qcheckbox.h>

/* 
 * 
 * Dialog for selecting input source file ,with extra options
 * 
 */
 
class ImportDialog : public QFileDialog
{
    Q_OBJECT

    public:
        ImportDialog(const QString &dirName, QWidget * parent);

    public:
        bool getCsvMode() const;

    private:
        QCheckBox *m_CsvModeCB;
};

#endif // IMPORTTDIALOG_H


