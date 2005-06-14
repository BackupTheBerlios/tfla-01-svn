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
#ifndef SETTINGS_H
#define SETTINGS_H

#include <qsettings.h>

class Settings
{
    public:
        Settings() throw ();
        virtual ~Settings() { }
        
        bool writeEntry(const QString & key, const QString & value) throw ();
        bool writeEntry(const QString & key, int value) throw ();
        bool writeEntry(const QString & key, bool value) throw ();
        bool writeEntry(const QString & key, double value) throw ();
        
        QString readEntry(const QString & key, const QString& def = QString::null) throw ();
        int readNumEntry (const QString & key, int def = 0) throw ();
        double readDoubleEntry(const QString & key, double def = 0.0) const throw ();
        bool readBoolEntry(const QString & key, bool def = false) const throw ();
        
     public:
        static Settings& set() throw ();
        
    private:
        QSettings               m_qSettings;
        QMap<QString, QString>  m_stringMap;
        QMap<QString, int>      m_intMap;
        QMap<QString, bool>     m_boolMap;
        QMap<QString, double>   m_doubleMap;
        
    private:
        Settings(const Settings&);
        Settings& operator=(const Settings&);
};

#endif /* SETTINGS_H */
