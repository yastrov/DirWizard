#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QDir>
#include <QList>
#include <QCryptographicHash>
#include <QDebug>
#include <QListWidgetItem>
#include <QCryptographicHash>
#include <QComboBox>
#include <QStringList>
#include <QTableWidgetItem>
#include <QFileInfo>
#include <QList>
#include <QVector>
#include <QHash>
#include <QFile>
#include <QFileDialog>
#include <QDirIterator>
#include <QDir>
#include <QMutableListIterator>
#include <QListIterator>
#include <hashfileinfostruct.h>
#include <duplicatefinder.h>
#include <dircomparator.h>
#include <QThread>
#include <QComboBox>
#include <QTableWidget>
#include <QListWidget>
#include <QRegExp>
#include <QStringListIterator>
#include <QStringList>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void stopThread();

private slots:
    void on_pushButton_Add_Dir_clicked();
    void on_pushButton_Cancel_clicked();
    void on_pushButton_Remove_Checked_clicked();
    void on_pushButton_Duplicate_Search_clicked();
    void on_pushButton_RemoveDir_clicked();
    void on_AboutAction_Triggered(bool checked);
    // Duplicate files search
    void showDuplicatesInTable(QList<HashFileInfoStruct> *items);
    // Comparing Folders
    void compareFoldersComplete(QList<HashFileInfoStruct> *items);
    void finishedThread();
    void on_pushButton_Compare_Folders_clicked();

private:
    Ui::MainWindow *ui;
    QString dirNameForFolderDialog;
    // ComboBox
    void addItemToComboBox(QString text, int data);
    void initHashComboBoxWidget();
    QCryptographicHash::Algorithm getCurrentHashAlgo();
    // ListWidget
    void initDirsListWidget();
    QList<QDir> getElementsFromDirsListWidget();
    void addToDirListWidget(const QString &dirName);
    // TableWidget

    QList<QString> getCheckedFileNamesFormTable();
    void initTableWidget();
    // Duplicate files search
    void startDuplicateSearchInBackground();
    // Comparing Folders
    void startComparingFoldersInBackground();
    // Other
};

#endif // MAINWINDOW_H
