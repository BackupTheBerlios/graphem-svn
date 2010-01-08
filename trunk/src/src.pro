TEMPLATE = app
DESTDIR = ../
TARGET = graphem

include(../common.pri)

HEADERS += auth.h inputwidget.h node.h mainwindow.h crypto.h newgesture.h preferences.h graphem.h
SOURCES += main.cpp graphem.cpp inputwidget.cpp mainwindow.cpp crypto.cpp preferences.cpp
