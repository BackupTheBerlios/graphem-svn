TEMPLATE      = lib
DESTDIR       = ../../../plugins
TARGET        = debug
CONFIG       += plugin

include(../../../common.pri)

HEADERS       = debug_auth.h \
	auth.h inputwidget.h
SOURCES       = debug_auth.cpp \
	inputwidget.cpp
