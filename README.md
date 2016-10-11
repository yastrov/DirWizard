# README

## About

Program with GUI for work with files: find unique, duplicates, calck and check hashes. Licensed under GPL v2.

## Motivation

No many programs for find duplicates files, and and even fewer programs to find unique files or automatically check hashes from multiple files. I fixed it:).

## Features
-  support many hash functions (MD5, SHA1, SHA256, SHA512, SHA3 256, SHA3 512)
-  work with many folders
-  find duplicates files and remove
-  find unique files in folders
-  calc hashes for files
-  check hashes for files
-  find empty folders and remove
-  Check zip files, show files. It shows files that can not be opened, or hash sum does not correspond to the content.

In Windows 10 start with admin rights, if you want to remove some things.

## Technology
-  Qt 5 with QThreads, signals and slots, QTableWidget, QComboBox Widget, QListWidget.
-  Qt Creator
-  c++ with some features from c++11
-  qt-opensource-windows-x86-mingw530-5.7.0

## Class Hierarchy

QMainWindow -> MainWindow
QObject -> DirWalker -> HashDirWalker
                |           |
                |           |->DirComparator
                |           |->DuplicateFinder
                |           |->CalcAndSaveHash
                |           |->LoadAndCheckHash
                |
                |-----> ZipWalkChecker
                |-----> EmptyDirFinder

QAbstractTableModel -> BaseTableModel -> DuplicatesTableModel
                            |
                            |----------> FileListTableModel
                            |----------> EmptyFolderTableModel

QDialog -> FiltersDialog

## Windows build instruction
I used MinGW 5.3.0 32 bit version. (From qt-opensource-windows-x86-mingw492-5.7.0 package.)  
Download zlib (for example [zlib128.zip](http://zlib.net/zlib128.zip) ) and extract to C:\Qt\zlib-1.2.8 :

Bat script:
    setlocal
    set PATH=C:\Qt\Qt5.7.0\Tools\mingw53_32\bin\;%PATH%
    cd C:\Qt\zlib-1.2.8
    mingw32-make -f win32/Makefile.gcc
    endlocal

Download [QuaZIP](http://sourceforge.net/projects/quazip/) and unpack to C:\Qt\quazip-0.7.2 .
Bat script:
    setlocal
    set PATH=C:\Qt\Qt5.7.0\Tools\mingw53_32\bin\;C:\Qt\Qt5.7.0\5.7\mingw53_32\bin;%PATH%
    cd C:\Qt\quazip-0.7.2
    qmake "CONFIG+=release" "INCLUDEPATH+=C:/Qt/zlib-1.2.8" "LIBS+=-LC:/Qt/zlib-1.2.8 -lz"
    mingw32-make
    qmake "CONFIG+=debug" "INCLUDEPATH+=C:/Qt/zlib-1.2.8" "LIBS+=-LC:/Qt/zlib-1.2.8 -lz"
    mingw32-make
    endlocal
