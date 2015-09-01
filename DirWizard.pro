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

SOURCES += main.cpp\
        mainwindow.cpp \
    hashdirwalker.cpp \
    duplicatefinder.cpp \
    dircomparator.cpp

HEADERS  += mainwindow.h \
    hashdirwalker.h \
    duplicatefinder.h \
    hashfileinfostruct.h \
    dircomparator.h

FORMS    += mainwindow.ui
