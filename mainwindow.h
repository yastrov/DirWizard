#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QDir>
#include <QList>
#include <QCryptographicHash>
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif
#include <QListWidgetItem>
#include <QCryptographicHash>
#include <QComboBox>
#include <QStringList>
#include <QTableWidgetItem>
#include <QHeaderView>
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
#include <QCloseEvent>
#include "calcandsavehash.h"
#include "loadandcheckhash.h"
#include "zipwalkchecker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void saveModelDataToFile(QString filename);

private slots:
#ifdef TABLEWIDGET_NO_MODEL
    void tableWidgetItemChanged(QTableWidgetItem * item);
    void tableWidget_header_clicked(int column);
#endif
    void on_pushButton_Add_Dir_clicked();
    void on_pushButton_Cancel_clicked();
    void on_pushButton_Remove_Checked_clicked();
    void on_pushButton_Duplicate_Search_clicked();
    void on_pushButton_Remove_Dir_clicked();
    void on_AboutAction_Triggered(bool checked);
    // Duplicate files search
    void showDuplicatesInTable(QSharedPtrListHFIS itemsPtr);
    // Comparing Folders
    void showUniqFilesInTable(QSharedPtrListHFIS itemsPtr);
    void finishedThread();
    void on_pushButton_Compare_Folders_clicked();
    void on_pushButton_Save_From_Table_clicked();
    void on_pushButton_Calc_Hashes_clicked();
    void on_pushButton_Check_Hashes_clicked();
    void showInvalidHashFilesInTable(QSharedPtrListHFIS itemsPtr);
    void on_pushButton_Check_Zip_clicked();
    void showInvalidZipInTable(QSharedPtrListHFIS itemsPtr);

private:
    Ui::MainWindow *ui;
    QString dirNameForFolderDialog;
    QThread* thread;
#ifdef TABLEWIDGET_NO_MODEL
    QSharedPtrListHFIS itemsResult;
#endif
    // ComboBox
    void addItemToComboBox(QString text, int data);
    void initHashComboBoxWidget();
    QCryptographicHash::Algorithm getCurrentHashAlgo();
    // ListWidget
    void initDirsListWidget();
    QList<QDir> getElementsFromDirsListWidget();
    void addToDirListWidget(const QString &dirName);
    // TableWidget
    void initTableWidget();
    // Duplicate files search
    void startDuplicateSearchInBackground();
    // Comparing Folders
    void startComparingFoldersInBackground();
    // Other
    void saveItemsToFile(const QString &fileName);
    void setUiPushButtonsEnabled(bool flag);
    // Calc hashes
    void startCalcHashesInBackground();
    void startCheckHashesInBackground();
    void startCheckZipsInBackground();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
};

#endif // MAINWINDOW_H
