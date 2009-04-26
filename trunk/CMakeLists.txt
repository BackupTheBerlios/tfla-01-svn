# (c) 2008, Bernhard Walle <bernhard.walle@gmx.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

PROJECT(tfla-01)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)


INCLUDE(FindQt4)
INCLUDE(CTest)
SET(QT_USE_QT3SUPPORT 1)
SET(QT_USE_QTMAIN     1) # only Windows
INCLUDE(${QT_USE_FILE})

# ieee1284
INCLUDE(Findieee1284)
INCLUDE_DIRECTORIES(${IEEE1284_INCLUDE_DIR})

# include the current working directory
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src)

# version
SET(VERSION_STRING  "0.2.0")
SET(MAJOR_VERSION   0)
SET(MINOR_VERSION   2)
SET(PATCH_VERSION   0)
ADD_DEFINITIONS(-DVERSION_STRING="${VERSION_STRING}")
ADD_DEFINITIONS(-DMAJOR_VERSION=${MAJOR_VERSION})
ADD_DEFINITIONS(-DMINOR_VERSION=${MINOR_VERSION})
ADD_DEFINITIONS(-DPATCH_VERSION=${PATCH_VERSION})

# always print compiler warnings
IF (CMAKE_HOST_UNIX)
    ADD_DEFINITIONS(-Wall -W -Wno-unused-parameter)
ENDIF (CMAKE_HOST_UNIX)

IF (MSVC)
    # MSVC++ ignores all exception specifications except '()' [i.e. no exception]
    # we can safely ignore that warning when compiling on MS Windows
    ADD_DEFINITIONS("/w44290")

    # we also don't want useless warnings that we should use some Windows specific functions
    # instead of Standard C functions (which is quite right because the standard C functions
    # are not safe, but we have no alternative in portable code)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS=1)

    # also get rid of warnings "Function call with parameters that may be unsafe"
    # the code is correct C++!
    ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS=1)
ENDIF (MSVC)


SET(tfla01_SRCS
    src/main.cpp
    src/hardware/parporterror.cpp
    src/hardware/parporterror.h
    src/hardware/parport.cpp
    src/hardware/parport.h
    src/hardware/parportlist.cpp
    src/hardware/parportlist.h
    src/triggerwidget.cpp
    src/settings.cpp
    src/settings.h
    src/centralwidget.cpp
    src/datacollector.cpp
    src/datacollector.h
    src/parameterbox.cpp
    src/dataplot.cpp
    src/aboutdialog.cpp
    src/help.cpp
    src/dataview.cpp
    src/data.cpp
    src/data.h
    src/tfla01.cpp
    src/exportdialog.cpp
    src/wheelscrollbar.cpp
)

SET(tfla01_MOCS
    src/triggerwidget.h
    src/centralwidget.h
    src/parameterbox.h
    src/dataplot.h
    src/aboutdialog.h
    src/help.h
    src/dataview.h
    src/tfla01.h
    src/exportdialog.h
    src/wheelscrollbar.h
)

# if (cmake_host_win32)
#     # copy icons
#     configure_file(
#        ${cmake_source_dir}/share/win32/qpamat_34.ico
#        ${cmake_binary_dir}/qpamat_34.ico
#        copyonly
#     )
#     configure_file(
#        ${cmake_source_dir}/share/win32/qpamat_16.ico
#        ${cmake_binary_dir}/qpamat_16.ico
#        copyonly
#     )
#     configure_file(
#        ${cmake_source_dir}/share/win32/qpamat_48.ico
#        ${cmake_binary_dir}/qpamat_48.ico
#        copyonly
#     )
# endif (cmake_host_win32)

# compiler for rc files
IF (MINGW)
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT
    "<CMAKE_RC_COMPILER> --output-format=coff <FLAGS> <DEFINES> -o <OBJECT> <SOURCE>")
ENDIF (MINGW)

# translations
QT4_CREATE_TRANSLATION(
    tfla01_qmfile
    ${CMAKE_SOURCE_DIR}/src
    share/tfla-01/translations/de.ts
)

# Compiled-in pixmaps
SET(QtApp_RCCS tfla-01.qrc)

# generate rules for building source files from the resources
QT4_ADD_RESOURCES(tfla01_RCC_SRCS tfla-01.qrc)

# generate rules for building source files that moc generates
QT4_WRAP_CPP(tfla01_MOC_SRCS ${tfla01_MOCS})

# build sources, moc'd sources, and rcc'd sources
ADD_EXECUTABLE(tfla-01 WIN32
    ${tfla01_SRCS} ${tfla01_MOC_SRCS} ${tfla01_RCC_SRCS} ${tfla01_qmfile}
)

TARGET_LINK_LIBRARIES(tfla-01 ${QT_LIBRARIES} ${IEEE1284_LIBRARIES})

#
# {{{ Build the Documentation
#

FIND_PROGRAM(XSLTPROC_EXECUTABLE xsltproc)

# create target directories
FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/doc/user/en)

ADD_CUSTOM_COMMAND(
    OUTPUT
        ${CMAKE_BINARY_DIR}/doc/user/en/index.html # creates more files
    COMMAND
        ${XSLTPROC_EXECUTABLE}
        ${CMAKE_SOURCE_DIR}/doc/user/tfla-01.xsl
        ${CMAKE_SOURCE_DIR}/doc/user/en/tfla-01.xml &&
        ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/doc/user/tfla-01.css
        ${CMAKE_BINARY_DIR}/doc/user/en/tfla-01.css
    DEPENDS
        ${CMAKE_SOURCE_DIR}/doc/user/en/tfla-01.xml
    WORKING_DIRECTORY
        ${CMAKE_BINARY_DIR}/doc/user/en
)

ADD_CUSTOM_TARGET(
    doc
    DEPENDS
        ${CMAKE_BINARY_DIR}/doc/user/en/index.html
)

ADD_DEPENDENCIES(
    tfla-01
    doc
)

# }}}

#
# {{{ Installation
#

INSTALL(
    TARGETS
        tfla-01
    DESTINATION
        bin
)

INSTALL(
    FILES
        ${tfla01_qmfile}
    DESTINATION
        share/tfla-01/translations
)

INSTALL(
    FILES
        ${CMAKE_SOURCE_DIR}/images/tfla-01_32.png
    DESTINATION
        share/pixmaps/
    RENAME
        tfla-01.png
)

# INSTALL(
#     FILES
#         ${CMAKE_SOURCE_DIR}/share/qpamat.desktop
#     DESTINATION
#         share/applications
# )

INSTALL(
    DIRECTORY
        ${CMAKE_BINARY_DIR}/doc/user/en
    DESTINATION
        share/tfla-01/doc/
    FILES_MATCHING REGEX ".*.(html|css)"
)

# }}}

# :tabSize=4:noTabs=true:indentSize=4:mode=cmake:folding=explicit:
# vim: set sw=4 ts=4 et fdm=marker:
