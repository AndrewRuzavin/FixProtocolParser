TEMPLATE = app
QT -= gui

CONFIG += c++14 console
CONFIG -= app_bundle
CONFIG += qt

PROJECT_PATH = "../FixProtocolParser"
PROJECT_SRC_PATH = "$${PROJECT_PATH}/src"

PATH_TO_INPUT_FILE = "$$PWD/Task/file1.txt"
DEFINES += PATH_TO_INPUT_FILE=\"\\\"$${PATH_TO_INPUT_FILE}\\\"\"

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/builds/debug
} else {
    DESTDIR = $$PWD/builds/release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.rcc
UI_DIR = $$DESTDIR/.ui
