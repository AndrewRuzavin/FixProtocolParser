include(../config.pri)
include(src.pri)

TARGET = FixProtocolParser

DEFINES += QT_DEPRECATED_WARNINGS

VPATH += ./src

SOURCES += \
	main.cpp
