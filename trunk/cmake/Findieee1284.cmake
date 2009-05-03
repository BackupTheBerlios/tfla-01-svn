# - Try to find ieee1284
# Once done this will define
#
#  IEEE1284_FOUND - system has ieee1284
#  IEEE1284_INCLUDE_DIRS - the ieee1284 include directory
#  IEEE1284_LIBRARIES - Link these to use ieee1284
#  IEEE1284_DEFINITIONS - Compiler switches required for using ieee1284
#
#  Copyright (c) 2009 Bernhard Walle <bernhard.walle@gmx.de>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (IEEE1284_LIBRARIES AND IEEE1284_INCLUDE_DIRS)
  # in cache already
  set(IEEE1284_FOUND TRUE)
else (IEEE1284_LIBRARIES AND IEEE1284_INCLUDE_DIRS)
  find_path(IEEE1284_INCLUDE_DIR
    NAMES
      ieee1284.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
      libieee1284/include
  )

  find_library(IEEE1284_LIBRARY
    NAMES
      ieee1284
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
      libieee1284
  )
  mark_as_advanced(IEEE1284_LIBRARY)

  if (IEEE1284_LIBRARY)
    set(IEEE1284_FOUND TRUE)
  endif (IEEE1284_LIBRARY)

  set(IEEE1284_INCLUDE_DIRS
    ${IEEE1284_INCLUDE_DIR}
  )

  if (IEEE1284_FOUND)
    set(IEEE1284_LIBRARIES
      ${IEEE1284_LIBRARIES}
      ${IEEE1284_LIBRARY}
    )
  endif (IEEE1284_FOUND)

  if (IEEE1284_INCLUDE_DIRS AND IEEE1284_LIBRARIES)
     set(IEEE1284_FOUND TRUE)
  endif (IEEE1284_INCLUDE_DIRS AND IEEE1284_LIBRARIES)

  if (IEEE1284_FOUND)
    if (NOT ieee1284_FIND_QUIETLY)
      message(STATUS "Found ieee1284: ${IEEE1284_LIBRARIES}")
    endif (NOT ieee1284_FIND_QUIETLY)
  else (IEEE1284_FOUND)
    if (ieee1284_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find ieee1284")
    endif (ieee1284_FIND_REQUIRED)
  endif (IEEE1284_FOUND)

  # show the IEEE1284_INCLUDE_DIRS and IEEE1284_LIBRARIES variables only in the advanced view
  mark_as_advanced(IEEE1284_INCLUDE_DIRS IEEE1284_INCLUDE_DIR IEEE1284_LIBRARIES)

endif (IEEE1284_LIBRARIES AND IEEE1284_INCLUDE_DIRS)

