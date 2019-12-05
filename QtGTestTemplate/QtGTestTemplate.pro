include(../config.pri)
include(src.pri)

TARGET = QtGTestTemplate

DEFINES += QT_DEPRECATED_WARNINGS

VPATH += ./src

SOURCES += \
	main.cpp \
