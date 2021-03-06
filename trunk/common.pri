#common settings for qmake

CONFIG += debug silent
QMAKE_CXXFLAGS_RELEASE += -D QT_NO_DEBUG -D QT_NO_DEBUG_OUTPUT
LIBS += -lX11

QCA_PATH = /usr/include/QtCrypto
exists($$QCA_PATH) {
	INCLUDEPATH += $$QCA_PATH
	LIBS += -lqca
} else {
	message("QCA library not found, disabling it")
	DEFINES += NO_QCA
}

OBJECTS_DIR = $${PWD}/src/tmp
MOC_DIR = $${PWD}/src/tmp
VPATH += $${PWD}/src
INCLUDEPATH += $${PWD}/src
