TEMPLATE      = lib
DESTDIR       = ../../../plugins
TARGET        = debug
CONFIG       += plugin

include(../../../common.pri)

unix {
	ARCH = $$system(uname -i)
	contains( ARCH, x86_64 ) {
		CXXFLAGS += -fPIC
		OBJECTS_DIR = $${PWD}/../tmp
	}
}

HEADERS       = debug_auth.h \
	auth.h inputwidget.h
SOURCES       = debug_auth.cpp \
	inputwidget.cpp
