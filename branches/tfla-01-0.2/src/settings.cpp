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
#include <qsettings.h>
#include <qdir.h>
#include <qapplication.h>

#include "settings.h"
#include "global.h"

// -------------------------------------------------------------------------------------------------
Settings::Settings()
    throw ()
{
    m_qSettings.setPath( "bwalle.de", "TFLogAnalizer", QSettings::User );
    m_qSettings.beginGroup("/tfla-01");
    
#define DEF_STRING(a, b) ( m_stringMap.insert( (a), (b) ) )
#define DEF_INTEGE(a, b) ( m_intMap.insert( (a), (b) ) )
#define DEF_DOUBLE(a, b) ( m_doubleMap.insert( (a), (b) ) )
#define DEF_BOOLEA(a, b) ( m_boolMap.insert( (a), (b) ) )
    
    DEF_BOOLEA("Main Window/maximized",          false);
    DEF_STRING("Main Window/SearchHistory",      "");
    DEF_STRING("Main Window/Layout",             "");
    DEF_INTEGE("Hardware/Parport",               0);
    DEF_INTEGE("Measuring/Triggering/Minutes",   0);
    DEF_INTEGE("Measuring/Triggering/Seconds",   5);
    DEF_INTEGE("Measuring/Triggering/Value",     0);
    DEF_INTEGE("Measuring/Triggering/Mask",      0xff);
    DEF_INTEGE("Measuring/Number_Of_Skips",      0);
    DEF_STRING("UI/Foreground_Color_Line",       "yellow");
    DEF_STRING("UI/Background_Color",            "black");
    DEF_STRING("UI/Left_Marker_Color",           "red");
    DEF_STRING("UI/Right_Marker_Color",          "green");
#ifdef Q_WS_WIN
    DEF_STRING("General/Webbrowser",             "explorer");
#else
    DEF_STRING("General/Webbrowser",             "firefox");
#endif
    
#undef DEF_STRING
#undef DEF_INTEGE
#undef DEF_DOUBLE
#undef DEF_BOOLEA
}


// -------------------------------------------------------------------------------------------------
bool Settings::writeEntry(const QString & key, const QString & value)
    throw ()
{
    return m_qSettings.writeEntry(key, value);
}


// -------------------------------------------------------------------------------------------------
bool Settings::writeEntry(const QString & key, double value)
    throw ()
{
    return m_qSettings.writeEntry(key, value);
}


// -------------------------------------------------------------------------------------------------
bool Settings::writeEntry(const QString & key, int value)
    throw ()
{
    return m_qSettings.writeEntry(key, value);
}


// -------------------------------------------------------------------------------------------------
bool Settings::writeEntry(const QString & key, bool value)
    throw ()
{
    return m_qSettings.writeEntry(key, value);
}


// -------------------------------------------------------------------------------------------------
QString Settings::readEntry(const QString & key, const QString& def)
    throw ()
{
    bool read = false;
    QString string = m_qSettings.readEntry(key, def, &read);
    if (!read && m_stringMap.contains(key))
    {
        return m_stringMap[key];
    }
#ifdef DEBUG
    else if (!read && def.isNull())
    {
        PRINT_DBG("Implicit default returned, key = %s", (const char*)key.local8Bit());
    }
#endif
    return string;
}


// -------------------------------------------------------------------------------------------------
int Settings::readNumEntry (const QString & key, int def)
    throw ()
{
    bool read = false;
    int number = m_qSettings.readNumEntry(key, def, &read);
    if (!read && m_intMap.contains(key))
    {
        return m_intMap[key];
    }
#ifdef DEBUG
    else if (!read && def == 0)
    {
        PRINT_TRACE("Implicit default returned, key = %s", key.latin1());
    }
#endif
    return number;
}


// -------------------------------------------------------------------------------------------------
double Settings::readDoubleEntry(const QString & key, double def) const
    throw ()
{
    bool read = false;
    double number = m_qSettings.readDoubleEntry(key, def, &read);
    if (!read && m_doubleMap.contains(key))
    {
        return m_doubleMap[key];
    }
#ifdef DEBUG
    else if (!read && def == 0.0)
    {
        PRINT_TRACE("Implicit default returned, key = %s", (const char*)key.local8Bit());
    }
#endif
    return number;
}


// -------------------------------------------------------------------------------------------------
bool Settings::readBoolEntry(const QString & key, bool def) const
    throw ()
{
    bool read = false;
    bool res = m_qSettings.readBoolEntry(key, def, &read);
    if (!read && m_boolMap.contains(key))
    {
        return m_boolMap[key];
    }
#ifdef DEBUG
    else if (!read && !def)
    {
        PRINT_TRACE("Implicit default returned, key = %s", (const char*)key.local8Bit());
    }
#endif
    return res;
}


// -------------------------------------------------------------------------------------------------
Settings& Settings::set()
    throw ()
{
    static Settings instanz;
    
    return instanz;
}


