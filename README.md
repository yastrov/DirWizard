# README

## About

Program with GUI for work with files: find unique, duplicates, calck and check hashes. Licensed under GPL v2.

## Motivation

No many programs for find duplicates files, and and even fewer programs to find unique files or automatically check hashes from multiple files. I fixed it:).

## Features
-  work with many folders
-  find duplicates files
-  find unique files in folders
-  calc hashes for files
-  check hashes for files

## Technology
-  QT 5 with QThreads, signals and slots, QTableWidget, QComboBox Widget, QListWidget.
-  QT Creator
-  c++ with some features from c++11
-  qt-opensource-windows-x86-mingw492-5.5.0

## Classes structure

QMainWindow -> MainWindow
QObject -> DirWalker -> HashDirWalker
                |           |
                |           |->DirComparator
                |           |->DuplicateFinder
                |           |->CalcAndSaveHash
                |           |->LoadAndCheckHash
