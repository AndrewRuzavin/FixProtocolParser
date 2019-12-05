include(gtest_dependency.pri)
include(../config.pri)
include($${PROJECT_PATH}/src.pri)

TARGET = QtGTestTemplateTest

CONFIG += thread

INCLUDEPATH += \
	$$PROJECT_SRC_PATH

VPATH += ./src

SOURCES += \
	src/main.cpp \
	SomeCodeTest.cpp \
