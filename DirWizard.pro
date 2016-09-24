#-------------------------------------------------
#
# Project created by QtCreator 2015-08-31T09:45:46
#
#-------------------------------------------------

QT       += core gui
QT += winextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DirWizard
TEMPLATE = app

debug {
  DEFINES += MYPREFIX_DEBUG
}

release {
  DEFINES += MYPREFIX_RELEASE
}

DEFINES *= QT_USE_QSTRINGBUILDER
CONFIG += c++11

win32: {
    INCLUDEPATH += C:/Qt/zlib-1.2.8
    INCLUDEPATH += C:/Qt/quazip-0.7.2
}

win32-g++: {
    LIBS += -LC:/Qt/zlib-1.2.8 -lz
    LIBS += -LC:/Qt/quazip-0.7.2/quazip/release -lquazip
}

win32-msvc: {
    LIBS += C:/Qt/zlib-1.2.8/zlib.lib
    LIBS += C:/Qt/quazip-0.7.2/quazip/release/quazip.lib
}

unix: {
    LIBS+=-L/usr/local/zlib/lib -lz
    INCLUDEPATH+=/usr/local/zlib/include

    LIBS += -L/usr/local/quazip-0.7.2/quazip/release -lquazip
    INCLUDEPATH += /usr/local/quazip-0.7.2
}

SOURCES += src/main.cpp\
        src/gui/mainwindow.cpp \
    src/worker/dirwalker.cpp \
    src/worker/hashdirwalker.cpp \
    src/worker/duplicatefinder.cpp \
    src/worker/dircomparator.cpp \
    src/worker/calcandsavehash.cpp \
    src/worker/loadandcheckhash.cpp \
    src/worker/zipwalkchecker.cpp \
    src/tablemodel/basetablemodel.cpp \
    src/tablemodel/duplicatestablemodel.cpp \
    src/tablemodel/filelisttablemodel.cpp \
    src/gui/dialog/filtersdialog.cpp

HEADERS  += src/gui/mainwindow.h \
    src/worker/dirwalker.h \
    src/worker/hashdirwalker.h \
    src/worker/duplicatefinder.h \
    src/hashfileinfostruct.h \
    src/worker/dircomparator.h \
    src/worker/calcandsavehash.h \
    src/worker/loadandcheckhash.h \
    src/worker/zipwalkchecker.h \
    src/tablemodel/basetablemodel.h \
    src/tablemodel/duplicatestablemodel.h \
    src/tablemodel/filelisttablemodel.h \
    src/gui/dialog/filtersdialog.h \
    src/constants.h

FORMS    += src/gui/mainwindow.ui

VERSION = 0.0.0.1
#QMAKE_TARGET_COMPANY = company
QMAKE_TARGET_PRODUCT = DirWizard
QMAKE_TARGET_DESCRIPTION = "Program for find duplicates or unique files, calculate and check hashes."
QMAKE_TARGET_COPYRIGHT = (c) Yuriy Astrov
# RC_ICONS = myappico.ico
# RC_FILE = myapp.rc
GITHUB_URL = "https://github.com/yastrov/DirWizard"

DEFINES += \
APP_VERSION=\"\\\"$$VERSION\\\"\" \
APP_URL=\"\\\"$$GITHUB_URL\\\"\" \

