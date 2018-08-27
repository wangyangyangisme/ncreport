QT = xml sql gui core
CONFIG += release \
          warn_on \
          qt \
          thread 
TEMPLATE = app 
HEADERS += ncreporttestform.h 
SOURCES += main.cpp \
           ncreporttestform.cpp
win32 {
	LIBS += ../bin/ncreport.lib 
}
unix: LIBS += -lncreport -L../lib -L/usr/local/bin 
TARGET = ncreportrunner
DESTDIR = ../bin
INSTALLS = target
target.path = /usr/local/bin
DESTDIR = ../bin
RC_FILE=ncreport.rc
