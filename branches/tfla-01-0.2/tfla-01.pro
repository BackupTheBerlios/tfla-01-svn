TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= warn_on qt exceptions thread

LIBS	+= -lqassistantclient

win32:LIBS	+= c:\openssl\lib\vc\ssleay32.lib c:\openssl\lib\vc\libeay32.lib

unix:LIBS	+= -lieee1284

DEFINES	+= VERSION_STRING=\"$$VERSION_STRING\"

INCLUDEPATH	+= src/

win32:INCLUDEPATH	+= c:\openssl\include

HEADERS	+= src/hardware/parport.h \
	src/hardware/parporterror.h \
	src/hardware/parportlist.h \
	src/triggerwidget.h \
	src/settings.h \
	src/centralwidget.h \
	src/datacollector.h \
	src/parameterbox.h \
	src/dataplot.h \
	src/aboutdialog.h \
	src/help.h \
	src/dataview.h \
	src/data.h \
	src/tfla01.h \
	src/exportdialog.h \
	src/importdialog.h \
	src/wheelscrollbar.h \
	src/VcdReader.h \
	src/VCD_symbol.h \
	src/io_vcl.h

SOURCES	+= src/main.cpp \
	src/hardware/parporterror.cpp \
	src/hardware/parport.cpp \
	src/hardware/parportlist.cpp \
	src/triggerwidget.cpp \
	src/settings.cpp \
	src/centralwidget.cpp \
	src/datacollector.cpp \
	src/parameterbox.cpp \
	src/dataplot.cpp \
	src/aboutdialog.cpp \
	src/help.cpp \
	src/dataview.cpp \
	src/data.cpp \
	src/tfla01.cpp \
	src/exportdialog.cpp \
	src/importdialog.cpp \
	src/wheelscrollbar.cpp \
	src/VcdReader.cpp \
	src/VCD_symbol.cpp \
	src/io_vcl.cpp 
	
IMAGES	= images/tfla-01_32.png \
	images/stock_exit_16.png \
	images/stock_exit_24.png \
	images/stock_help_16.png \
	images/stock_help_24.png \
	images/info_16.png \
	images/info_24.png \
	images/whats_this.png \
	images/qt_16.png \
	images/stock_redo_16.png \
	images/stock_redo_24.png \
	images/stock_stop_16.png \
	images/stock_stop_24.png \
	images/stock_zoom-1_16.png \
	images/stock_zoom-1_24.png \
	images/stock_zoom_in_16.png \
	images/stock_zoom_in_24.png \
	images/stock_zoom-out_16.png \
	images/stock_zoom-out_24.png \
	images/stock_zoom-page-width_16.png \
	images/stock_zoom-page-width_24.png \
	images/stock_3d-color-picker_16.png \
	images/stock_3d-color-picker_24.png \
	images/stock_last_16.png \
	images/stock_last_24.png \
	images/stock_first_16.png \
	images/stock_first_24.png \
	images/stock_right_arrow_16.png \
	images/stock_right_arrow_24.png \
	images/stock_left_arrow_16.png \
	images/stock_left_arrow_24.png \
	images/stock_convert_16.png \
	images/stock_convert_24.png \
	images/stock_zoom-optimal_16.png \
	images/stock_zoom-optimal_24.png \
	images/network_16.png \
	images/network_24.png \
	images/stock_previous-page_16.png \
	images/stock_previous-page_24.png \
	images/stock_next-page_16.png \
	images/stock_next-page_24.png \
	images/stock_export_16.png \
	images/stock_export_24.png

#
#  Use "qmake -o Makefile tfla-01.pro" for release code, and
#  use "qmake -o Makefile tfla-01.pro debug=1" for debug code.
#
#  Adapt these variables to your system

isEmpty(PREFIX) {
  PREFIX             = /usr/local
}
SHAREDIR           = $${PREFIX}/share
DOCDIR             = $${SHAREDIR}/doc/packages/tfla-01
BINDIR             = $${PREFIX}/bin

################################################################################

# On Windows, this is the default installation path of ieee1284. Change this
# if needed. 
# TODO: Change for that ... (this is partly OpenSSL)

################################################################################

VERSION_STRING     = 0.2.0
MAJOR_VERSION       = 0
MINOR_VERSION      = 2
PATCH_VERSION       = 0

################################################################################



# -----------------------------------------------------------------------------


# -----------------------------------------------------------------------------

DOC_LANGS    = en
TRANSLATIONS =                                  \
    share/tfla-01/translations/tfla-01_de.ts \
    share/tfla-01/translations/tfla-01_pl_PL.ts

# -----------------------------------------------------------------------------

UI_DIR = out

    

#win32:RC_FILE = share/win32/tfla-01_win32.rc


# -----------------------------------------------------------------------------


# -----------------------------------------------------------------------------

DEFINES    += MAJOR_VERSION=$$MAJOR_VERSION
DEFINES    += MINOR_VERSION=$$MINOR_VERSION
DEFINES    += PATCH_VERSION=$$PATCH_VERSION

isEmpty (debug) {
  #DEFINES  += QT_NO_CHECK
  CONFIG    += release
} else {
  CONFIG   += debug
  DEFINES  += DEBUG
  DEFINES  += TRACE
}

# -----------------------------------------------------------------------------

MOC_DIR     = out/
OBJECTS_DIR = out/
DESTDIR     = bin

# -----------------------------------------------------------------------------

#
# make a tarball on Unix with "make tarball"
maketarball.target          = tarball
maketarball.commands        = svn export `pwd` /tmp/tfla-01-$$VERSION_STRING &&
maketarball.commands       += pushd . &&
maketarball.commands       += cd /tmp/tfla-01-$$VERSION_STRING &&
maketarball.commands       += qmake tfla-01.pro &&
maketarball.commands       += make documentation &&
maketarball.commands       += rm Makefile &&
maketarball.commands       += cd /tmp &&
maketarball.commands       += tar cvfj tfla-01-$${VERSION_STRING}.tar.bz2 tfla-01-$$VERSION_STRING &&
maketarball.commands       += popd &&
maketarball.commands       += mv /tmp/tfla-01-$${VERSION_STRING}.tar.bz2 . &&
maketarball.commands       += rm -r /tmp/tfla-01-$$VERSION_STRING
QMAKE_EXTRA_UNIX_TARGETS   += maketarball

#
# make documentation on Unix with "make documentation"
makedoc.target              = documentation
makedoc.commands           += cd doc/user &&
makedoc.commands           += for i in $$DOC_LANGS ; do xsltproc -o \$$i/ tfla-01.xsl \$$i/tfla-01.xml ; done &&
makedoc.commands           += cd -
QMAKE_EXTRA_UNIX_TARGETS   += makedoc


# -----------------------------------------------------------------------------

#
# Installation

# install the documentation twice for convenience on Unix
unix:!mac {
  i_documentation.path        = $$DOCDIR
  i_documentation.files      += README COPYING ChangeLog
  i_documentation.files      += doc/user/* doc/schematic
  INSTALLS                   += i_documentation
}

i_userdoc.path              = $$SHAREDIR/tfla-01/doc/
i_userdoc.files            += doc/user/* doc/schematic
INSTALLS                   += i_userdoc

i_binary.path               = $$BINDIR
i_binary.files             += bin/tfla-01
INSTALLS                   += i_binary

i_translation.path          = $$SHAREDIR/tfla-01/translations
i_translation.files        += share/tfla-01/translations/*.qm
INSTALLS                   += i_translation

i_share.path                = $$SHAREDIR/tfla-01/
i_share.files              += COPYING
INSTALLS                   += i_share

i_pixmaps.path              = $$SHAREDIR/pixmaps
i_pixmaps.files            += images/tfla-01*
INSTALLS                   += i_pixmaps


