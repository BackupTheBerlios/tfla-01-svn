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
#ifndef GLOBAL_H
#define GLOBAL_H
#include <qvaluevector.h>

/**
 * uint is shorter than unsigned int.
 */
typedef unsigned int  uint;

typedef unsigned char uchar;

typedef long long sample_time_t;

typedef bool valid_sample_pos(sample_time_t x);
#define valid_sample_pos(a) ((a) >= 0)
#define SAMPLE_TIME_BELOW -1
#define SAMPLE_TIME_ABOWE -2

#define NUMBER_OF_WIRE_PER_SAPMPLE 8



// Copyright (c) 2003 Benedikt Meurer (benedikt.meurer@unix-ag.uni-siegen.de)
// xfwm4: debug.h


#if !defined(DOXYGEN) && (defined(DEBUG) || defined(TRACE))

#if defined(__NetBSD__) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
#define __DBG_FUNC__    __func__
#elif defined(__GNUC__) && __GNUC__ >= 3
#define __DBG_FUNC__	__FUNCTION__
#else
#define __DBG_FUNC__    "??"
#endif

#endif // !defined(DOXYGEN) && (defined(DEBUG) || defined(TRACE))


#ifdef DEBUG

#  ifdef _MSC_VER
#    define PRINT_DBG(fmt, args)                                  \
     {                                                            \
         qDebug("DEBUG[%s:%d] %s(): "fmt, __FILE__, __LINE__,     \
                 __DBG_FUNC__, ##args);                           \
     }
#  else
#    define PRINT_DBG(fmt, ...)                                   \
     {                                                            \
         qDebug("DEBUG[%s:%d] %s(): "fmt, __FILE__, __LINE__,     \
                __DBG_FUNC__, ## __VA_ARGS__);                    \
     }

#  endif

#else  

#  ifdef _MSC_VER
#    define PRINT_DBG(fmt, args)   { do {} while(0); }
#  else
#    define PRINT_DBG(fmt, ...)   { do {} while(0); }
#  endif

#endif


#ifdef TRACE

#  ifdef _MSC_VER

#    define PRINT_TRACE(fmt, args)                                \
     {                                                            \
         qDebug("TRACE[%s:%d] %s(): "fmt, __FILE__, __LINE__,     \
                 __DBG_FUNC__, ##args);                           \
     }

#    define PRINT_TRACE2(fmt, arg1,args,)                         \
     {                                                            \
         qDebug("TRACE[%s:%d] %s(): "fmt, __FILE__, __LINE__,     \
                 __DBG_FUNC__,arg1, ##args);                      \
     }

# else

#    define PRINT_TRACE(fmt, ...)                                 \
     {                                                            \
         qDebug("TRACE[%s:%d] %s(): "fmt, __FILE__, __LINE__,     \
                __DBG_FUNC__, ## __VA_ARGS__);                    \
     }

#    define PRINT_TRACE2(fmt, ...)                                \
     {                                                            \
         qDebug("TRACE[%s:%d] %s(): "fmt, __FILE__, __LINE__,     \
                __DBG_FUNC__, ## __VA_ARGS__);                    \
     }

# endif

#else  

#  ifdef _MSC_VER
#    define PRINT_TRACE(fmt, args)      { do {} while(0); }
#    define PRINT_TRACE2(fmt,arg, args) { do {} while(0); }
#  else
#    define PRINT_TRACE(fmt, ...)    { do {} while(0); }
#    define PRINT_TRACE2(fmt, ...)   { do {} while(0); }
#  endif

#endif

#define DABS(a) (((a) < 0) ? (-(a)) : (a))


#endif /* GLOBAL_H */
