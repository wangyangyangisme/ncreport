QT = xml sql gui core
TEMPLATE = lib
CONFIG += release \
          warn_on \
          qt \
          thread 
TARGET = ncreport
win32 {
    DESTDIR = ../bin
} else {
	DESTDIR = ../lib
}
DEPENDPATH += .
INCLUDEPATH += .
MOC_DIR = moc
OBJECTS_DIR = obj
RCC_DIR = rcc
UI_DIR = uic
target.path = /usr/lib
headers.files = ncreport.h ncsqlquery.h
headers.path = /usr/include
INSTALLS = target headers
# Input
HEADERS += globals.h \
           nciconfactory.h \
           nciconfactorybase.h \
           ncpreview.h \
           ncreport.h \
           ncsqlquery.h \
           ncutils.h \
           reportparser.h
SOURCES += nciconfactory.cpp \
           nciconfactorybase.cpp \
           ncpreview.cpp \
           ncreport.cpp \
           ncsqlquery.cpp \
           ncutils.cpp \
           reportparser.cpp
TRANSLATIONS += ncreport_de.ts ncreport_es.ts ncreport_hu.ts
DEFINES +=_EXPORTING_NCREPORT
RESOURCES = ncreport.qrc