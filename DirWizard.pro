#-------------------------------------------------
#
# Project created by QtCreator 2015-08-31T09:45:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DirWizard
TEMPLATE = app

debug {
  DEFINES += MYPREFIX_DEBUG
}

release {
  DEFINES += MYPREFIX_RELEASE
}

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    dirwalker.cpp \
    hashdirwalker.cpp \
    duplicatefinder.cpp \
    dircomparator.cpp \
    calcandsavehash.cpp \
    loadandcheckhash.cpp \

HEADERS  += mainwindow.h \
    dirwalker.h \
    hashdirwalker.h \
    duplicatefinder.h \
    hashfileinfostruct.h \
    dircomparator.h \
    calcandsavehash.h \
    loadandcheckhash.h \

FORMS    += mainwindow.ui

VERSION = 0.0.0.1
#QMAKE_TARGET_COMPANY = company
QMAKE_TARGET_PRODUCT = DirWizard
QMAKE_TARGET_DESCRIPTION = "Program for find duplicates or unique files, calculate and check hashes."
QMAKE_TARGET_COPYRIGHT = (c) Yuriy Astrov
GITHUB_URL = "https://github.com/yastrov/DirWizard"

DEFINES += \
APP_VERSION=\"\\\"$$VERSION\\\"\" \
APP_URL=\"\\\"$$GITHUB_URL\\\"\" \

