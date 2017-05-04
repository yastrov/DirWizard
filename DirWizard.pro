#-------------------------------------------------
#
# Project created by QtCreator 2015-08-31T09:45:46
#
#-------------------------------------------------

QT       += core gui

win32: {
    QT += winextras
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DirWizard
TEMPLATE = app

CONFIG(release, debug|release): {
    DEFINES += QT_NO_DEBUG_OUTPUT
    DEFINES += MYPREFIX_RELEASE
} else {
    DEFINES += MYPREFIX_DEBUG
}

DEFINES *= QT_USE_QSTRINGBUILDER
CONFIG += c++11

win32-g++: {
    DEFINES += USE_WIN_EXTRAS
    INCLUDEPATH += C:/Qt/zlib-1.2.11
    INCLUDEPATH += C:/Qt/quazip-0.7.3
    LIBS += -LC:/Qt/zlib-1.2.11 -lz
    LIBS += -LC:/Qt/quazip-0.7.3/quazip/release -lquazip
}

win32-msvc: {
    LIBS += C:/Qt/zlib-1.2.8/zlib.lib
    LIBS += C:/Qt/quazip-0.7.2/quazip/release/quazip.lib
}

unix: {
    CONFIG += link_pkgconfig
    PKGCONFIG += zlib

    LIBS += -L/usr/lib -lquazip5
    INCLUDEPATH += /usr/include/quazip5
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
    src/tablemodel/emptyfoldertablemodel.cpp \
    src/worker/emptydirfinder.cpp \
    src/gui/dialog/filtersdialog.cpp \
    src/settingshelper.cpp

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
    src/tablemodel/emptyfoldertablemodel.h \
    src/worker/emptydirfinder.h \
    src/gui/dialog/filtersdialog.h \
    src/constants.h \
    src/helper.h \
    src/settingshelper.h

FORMS    += src/gui/mainwindow.ui

TRANSLATIONS += translations/dirwizard_ru.ts

VERSION = 0.0.0.6
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

