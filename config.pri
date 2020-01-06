TEMPLATE = app
QT -= gui

CONFIG += c++14 console
CONFIG -= app_bundle
CONFIG -= qt

PROJECT_SRC_PATH = "$${PWD}/src"

INPUT_FILE = "file1.txt"

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/builds/debug
    PATH_TO_INPUT_FILE = "$$PWD/Task/$$INPUT_FILE"
    DEFINES += PATH_TO_INPUT_FILE=\"\\\"$${PATH_TO_INPUT_FILE}\\\"\"
} else {
    DESTDIR = $$PWD/builds/release
    DEFINES += PATH_TO_INPUT_FILE=\"\\\"$$INPUT_FILE\\\"\"
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.rcc
UI_DIR = $$DESTDIR/.ui
