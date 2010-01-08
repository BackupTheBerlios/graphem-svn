TEMPLATE      = lib
DESTDIR       = ../../../plugins
TARGET        = $$qtLibraryTarget(fcc)
CONFIG       += plugin

include(../../../common.pri)

HEADERS       = fcc_auth.h fcc_newgesture.h fcc_stroke.h \
	auth.h crypto.h inputwidget.h newgesture.h
SOURCES       = fcc_auth.cpp fcc_newgesture.cpp fcc_stroke.cpp \
	inputwidget.cpp crypto.cpp
