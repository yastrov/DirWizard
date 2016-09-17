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
    INCLUDEPATH += C:/Qt/quazip-0.7.1
}

win32-g++: {
    LIBS += -LC:/Qt/zlib-1.2.8 -lz
    LIBS += -LC:/Qt/quazip-0.7.1/quazip/release -lquazip
}

win32-msvc: {
    LIBS += C:/Qt/zlib-1.2.8/zlib.lib
    LIBS += C:/Qt/quazip-0.7.1/quazip/release/quazip.lib
}

unix: {
    LIBS+=-L/usr/local/zlib/lib -lz
    INCLUDEPATH+=/usr/local/zlib/include

    LIBS += -L/usr/local/quazip-0.7.1/quazip/release -lquazip
    INCLUDEPATH += /usr/local/quazip-0.7.1
}

SOURCES += main.cpp\
        mainwindow.cpp \
    dirwalker.cpp \
    hashdirwalker.cpp \
    duplicatefinder.cpp \
    dircomparator.cpp \
    calcandsavehash.cpp \
    loadandcheckhash.cpp \
    zipwalkchecker.cpp \
    basetablemodel.cpp \
    duplicatestablemodel.cpp \
    filelisttablemodel.cpp \
    filtersdialog.cpp

HEADERS  += mainwindow.h \
    dirwalker.h \
    hashdirwalker.h \
    duplicatefinder.h \
    hashfileinfostruct.h \
    dircomparator.h \
    calcandsavehash.h \
    loadandcheckhash.h \
    zipwalkchecker.h \
    basetablemodel.h \
    duplicatestablemodel.h \
    filelisttablemodel.h \
    filtersdialog.h \
    constants.h

FORMS    += mainwindow.ui

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

