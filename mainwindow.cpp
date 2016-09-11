#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "duplicatestablemodel.h"
#include "filelisttablemodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dirNameForFolderDialog(QDir::current().dirName()),
    thread(new QThread(this))
{
    ui->setupUi(this);
    initDirsListWidget();
    initHashComboBoxWidget();
    initTableWidget();
    QObject::connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::on_AboutAction_Triggered);
    QObject::connect(ui->pushButton_Calc_Hashes, &QPushButton::clicked, this, &MainWindow::on_pushButton_Calc_Hashes_clicked);
    //QObject::connect(ui->pushButton_Check_Hashes, &QPushButton::clicked, this, &MainWindow::on_pushButton_Check_Hashes_clicked);
    //QObject::connect(ui->pushButton_Check_Zip, &QPushButton::clicked, this, &MainWindow::on_pushButton_Check_Zip_clicked);

    // Drag and Drop
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Dirs List Widget Start
void MainWindow::initDirsListWidget()
{
   ui->listWidget->clear();
}

QList<QDir> MainWindow::getElementsFromDirsListWidget()
{
    int count = ui->listWidget->count();
    QListWidgetItem *item;
    QList<QDir> dirList;
    dirList.reserve(count);
    QDir dir;

    QString filterS = ui->lineEdit_Filter->text();
    QStringList qsl;
    if(!filterS.isNull() && !filterS.isEmpty())
    {
        qsl = filterS.split(QRegExp("\\s"));
#ifdef MYPREFIX_DEBUG
        QStringListIterator i(qsl);
        while(i.hasNext())
            qDebug()<< i.next();
#endif
    }

    QListWidget *list = ui->listWidget;
    for(int i=0; i < count; ++i)
    {
        item = list->item(i);
        dir = QDir(item->data(Qt::UserRole).toString());
        if(!qsl.empty()){
            dir.setNameFilters(qsl);
        }
        dirList.append(dir);
    }
    return dirList;
}

void MainWindow::addToDirListWidget(const QString &dirName)
{
    QListWidgetItem *item = new QListWidgetItem(QDir(dirName).absolutePath());
    QVariant variant(dirName);
    item->setData(Qt::UserRole, variant);
    item->setToolTip(dirName);
    ui->listWidget->addItem(item);
}
// END

// Hashes ComboBox
void MainWindow::initHashComboBoxWidget()
{
    addItemToComboBox(QString("Md5"), QCryptographicHash::Md5);
    addItemToComboBox(QString("Sha1"), QCryptographicHash::Sha1);
    addItemToComboBox(QString("Sha256"), QCryptographicHash::Sha256);
    addItemToComboBox(QString("Sha512"), QCryptographicHash::Sha512);
    addItemToComboBox(QString("Sha3_256"), QCryptographicHash::Sha3_256);
    addItemToComboBox(QString("Sha3_512"), QCryptographicHash::Sha3_512);
}

void MainWindow::addItemToComboBox(QString text, int data)
{
    ui->comboBox->addItem(text, QVariant(data));
}

QCryptographicHash::Algorithm MainWindow::getCurrentHashAlgo()
{
    QVariant variant = ui->comboBox->currentData();
    return static_cast<QCryptographicHash::Algorithm>(variant.toUInt());
}
// END


// Table Widget
void MainWindow::initTableWidget()
{
    QTableView *table = ui->tableView;
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(table, &QTableView::customContextMenuRequested, this, &MainWindow::createCustomPopupMenuForTableView);
}

void MainWindow::showDuplicatesInTable(QSharedPtrListHFIS itemsPtr)
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "showDuplicatesInTable";
#endif
    QTableView *table = ui->tableView;
    if(table->model() != nullptr)
        table->model()->deleteLater();
    DuplicatesTableModel *model = new DuplicatesTableModel(itemsPtr, this);
    table->setModel(model);
    table->setSortingEnabled(true);
    table->sortByColumn(DuplicatesTableModel::Column::groupId);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}
/*
QList<QString> MainWindow::getCheckedFileNamesFormTable()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "getCheckedFileNamesFormTable";
#endif
    QList<QString> result;
    int rowCount = ui->tableWidget->rowCount();
    QTableWidgetItem *item;
    QTableWidget *table = ui->tableWidget;
    for(int row=0; row<rowCount; row++)
    {
        item = table->item(row, 0);
        if( item->checkState() == Qt::Checked )
        {
            QVariant data = item->data(Qt::UserRole);
            result.append(data.toString());
        }
    }
    return result;
}
*/
// END

// Duplicate Files Search
void MainWindow::startDuplicateSearchInBackground()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "startDuplicateSearchInBackground";
#endif
    QList<QDir> dirs = getElementsFromDirsListWidget();
    if(!dirs.isEmpty() && !thread->isRunning())
    {
        DuplicateFinder *worker = new DuplicateFinder(getCurrentHashAlgo(), nullptr);

        worker->setQDir(dirs);
        worker->moveToThread(thread);
        //connect(thread, SIGNAL(started()), worker, SLOT(process()));
        QObject::connect(thread, &QThread::started, worker, &DuplicateFinder::process);
        //connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
        /*No examples where QThread::finished connected to &QThread::quit !
         * And then we need Worker::finished. */
        QObject::connect(worker, &DuplicateFinder::finished, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, worker, &DuplicateFinder::deleteLater);//From Off documentation
        QObject::connect(worker, &DuplicateFinder::finished, this, &MainWindow::finishedThread);
        //connect(worker, SIGNAL(finishedWData(QList<HashFileInfoStruct> *)), this, SLOT(showDuplicatesInTable(QList<HashFileInfoStruct> *)));
        QObject::connect(worker, &DuplicateFinder::finishedWData, this, &MainWindow::showDuplicatesInTable);
#ifdef MYPREFIX_DEBUG
        qDebug() << "startThread";
#endif
        thread->start();
    }
}

// End

// Complete folders
/*
 *
*/
void MainWindow::showUniqFilesInTable(QSharedPtrListHFIS itemsPtr)
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "showUniqFilesInTable";
#endif
    QTableView *table = ui->tableView;
    if(table->model() != nullptr)
        table->model()->deleteLater();
    FileListTableModel *model = new FileListTableModel(itemsPtr, this);
    table->setModel(model);
    table->setSortingEnabled(true);
    table->sortByColumn(FileListTableModel::Column::fileName);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::startComparingFoldersInBackground()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "startComparingFoldersInBackground";
#endif
    QList<QDir> dirs = getElementsFromDirsListWidget();
    if(!dirs.isEmpty() && !thread->isRunning())
    {
        DirComparator *worker = new DirComparator(getCurrentHashAlgo(), nullptr);

        worker->setQDir(dirs);
        worker->moveToThread(thread);
        //connect(thread, SIGNAL(started()), worker, SLOT(process()));
        QObject::connect(thread, &QThread::started, worker, &DirComparator::process);
        //connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
        QObject::connect(worker, &DirComparator::finished, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, worker, &DirComparator::deleteLater);//From Off documentation
        QObject::connect(worker, &DirComparator::finished, this, &MainWindow::finishedThread);
        //connect(worker, SIGNAL(finishedWData(QList<HashFileInfoStruct> *)), this, SLOT(compareFoldersComplete(QList<HashFileInfoStruct> *)));
        QObject::connect(worker, &DirComparator::finishedWData, this, &MainWindow::showUniqFilesInTable);
#ifdef MYPREFIX_DEBUG
        qDebug() << "startThread";
#endif
        thread->start();
    }
}
// END

/*
 * Open Folder Choose dialog and add Dirname to ListWidget
 */
void MainWindow::on_pushButton_Add_Dir_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "on_pushButton_Add_Dir_clicked";
#endif
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 dirNameForFolderDialog,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(!dirName.isEmpty() && !dirName.isNull())
    {
        dirNameForFolderDialog = QFileInfo(dirName).absoluteDir().absolutePath();
        addToDirListWidget(dirName);
    }
}

/*
 * Remove Seleccted item from TableWidget
 */

void MainWindow::on_pushButton_Remove_Checked_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << ":on_pushButton_Remove_Checked_clicked";
#endif
    if(ui->tableView->model()!=nullptr)
    qobject_cast<BaseTableModel*>(ui->tableView->model())->removeCheckedFunc();
}


/*
 * Slot for Start Duplicate file Search in background (other function)
 */
void MainWindow::on_pushButton_Duplicate_Search_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << "\n";
    qDebug() << "QThread: " << thread << "\n";
#endif
    if (thread->isRunning()) {
        thread->wait();
    }
    //ClearItemsResultStore();
    startDuplicateSearchInBackground();
    setUiPushButtonsEnabled(!thread->isRunning());
}

/*
 * Remove Seleccted item (Dir Name) from ListWidget
 */
void MainWindow::on_pushButton_Remove_Dir_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "on_pushButton_RemoveDir_clicked";
#endif
    // Din't work
    //ui->listWidget->removeItemWidget(ui->listWidget->currentItem());
    delete ui->listWidget->currentItem();
}

void MainWindow::on_pushButton_Cancel_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "on_pushButton_Cancel_clicked";
#endif
    thread->requestInterruption();
}

void MainWindow::on_AboutAction_Triggered(bool checked)
{
    Q_UNUSED(checked)
    QMessageBox::about(this,
                       tr("About DirWizard"),
                       tr("<h2>DirWizard</h2>"
                          "<p>Written by Yuriy (Yuri) Astrov<br/>"
                          "Based on QT 5<br/>"
                          "Licensed by GPLv2<br/>"
                          "Version: %1<br/>"
                          "URL: <a href='%2'>%2</a><p>").arg(APP_VERSION, APP_URL));

 }

void MainWindow::finishedThread()
{
    setUiPushButtonsEnabled(true);
    QMessageBox::information(this,
                             "DirWizard",
                             tr("Task completed successfully!"));
}


void MainWindow::on_pushButton_Compare_Folders_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << "\n";
    qDebug() << "QThread: " << thread << "\n";
#endif
    if (thread->isRunning()) {
        thread->wait();
    }
    //ClearItemsResultStore();
    startComparingFoldersInBackground();
    setUiPushButtonsEnabled(!thread->isRunning());
}

void MainWindow::on_pushButton_Save_From_Table_clicked()
{
    QString filter = "Text files (*.txt)";
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save File"),
                                     QDir::homePath(),
                                     "Text files (*.txt)",
                                     &filter, QFileDialog::DontUseNativeDialog);
#ifdef MYPREFIX_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << "\n";
    qDebug() << "File: " << fileName << "\n";
#endif
    if(!fileName.isNull() && !fileName.isEmpty())
    {
        QFileInfo qF(fileName);
        if(qF.exists())
        {
            QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                      "DirWizard",
                                                                      tr("File already exists!\nOverwrite?"),
                                                                      QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::No)
            {
                return;
            }
        }
        //emit saveModelDataToFile(fileName); But error with it.
        if(ui->tableView->model()!=nullptr)
        qobject_cast<BaseTableModel*>(ui->tableView->model())->saveToFileFunc(fileName);
    }
}

/*
    Calc Hashes
*/
void MainWindow::startCalcHashesInBackground()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "startCalcHashesInBackground";
#endif
    QList<QDir> dirs = getElementsFromDirsListWidget();
    if(!dirs.isEmpty() && !thread->isRunning())
    {
        CalcAndSaveHash *worker = new CalcAndSaveHash(getCurrentHashAlgo(), nullptr);

        worker->setQDir(dirs);
        worker->moveToThread(thread);
        QObject::connect(thread, &QThread::started, worker, &CalcAndSaveHash::process);
        QObject::connect(worker, &CalcAndSaveHash::finished, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, worker, &CalcAndSaveHash::deleteLater);//From Off documentation
        QObject::connect(worker, &CalcAndSaveHash::finished, this, &MainWindow::finishedThread);
#ifdef MYPREFIX_DEBUG
        qDebug() << "startThread";
#endif
        thread->start();
    }
}

void MainWindow::on_pushButton_Calc_Hashes_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << "\n";
    qDebug() << "QThread: " << thread << "\n";
#endif
    if (thread->isRunning()) {
        thread->wait();
    }
    startCalcHashesInBackground();
    setUiPushButtonsEnabled(!thread->isRunning());
}

/*
Checked Hashes
*/
void MainWindow::on_pushButton_Check_Hashes_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << "\n";
    qDebug() << "QThread: " << thread << "\n";
#endif
    if (thread->isRunning()) {
        thread->wait();
    }
    startCheckHashesInBackground();
    setUiPushButtonsEnabled(!thread->isRunning());
}

void MainWindow::startCheckHashesInBackground()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "startCheckHashesInBackground";
#endif
    QList<QDir> dirs = getElementsFromDirsListWidget();
    if(!dirs.isEmpty() && !thread->isRunning())
    {
        LoadAndCheckHash *worker = new LoadAndCheckHash(getCurrentHashAlgo(), nullptr);

        worker->setQDir(dirs);
        worker->moveToThread(thread);
        QObject::connect(thread, &QThread::started, worker, &LoadAndCheckHash::process);
        QObject::connect(worker, &LoadAndCheckHash::finished, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, worker, &LoadAndCheckHash::deleteLater);//From Off documentation
        QObject::connect(worker, &LoadAndCheckHash::finished, this, &MainWindow::finishedThread);
        QObject::connect(worker, &LoadAndCheckHash::finishedWData, this, &MainWindow::showInvalidHashFilesInTable);
#ifdef MYPREFIX_DEBUG
        qDebug() << "startThread";
#endif
        thread->start();
    }
}

void MainWindow::showInvalidHashFilesInTable(QSharedPtrListHFIS itemsPtr)
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "MainWindow::showInvalidHashFilesInTable";
#endif
    QTableView *table = ui->tableView;
    if(table->model() != nullptr)
        table->model()->deleteLater();
    FileListTableModel *model = new FileListTableModel(itemsPtr, this);
    table->setModel(model);
    table->setSortingEnabled(true);
    table->sortByColumn(FileListTableModel::Column::fileName);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::on_pushButton_Check_Zip_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "MainWindow::on_pushButton_Check_Zip_clicked";
#endif
    startCheckZipsInBackground();
    setUiPushButtonsEnabled(!thread->isRunning());
}

void MainWindow::startCheckZipsInBackground()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "MainWindow::startCheckZipsInBackground";
#endif
    QList<QDir> dirs = getElementsFromDirsListWidget();
    if(!dirs.isEmpty() && !thread->isRunning())
    {
        ZipWalkChecker *worker = new ZipWalkChecker(nullptr);

        worker->setQDir(dirs);
        worker->moveToThread(thread);
        QObject::connect(thread, &QThread::started, worker, &ZipWalkChecker::process);
        QObject::connect(worker, &ZipWalkChecker::finished, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, worker, &ZipWalkChecker::deleteLater);//From Off documentation
        QObject::connect(worker, &ZipWalkChecker::finished, this, &MainWindow::finishedThread);
        QObject::connect(worker, &ZipWalkChecker::finishedWData, this, &MainWindow::showInvalidZipInTable);
#ifdef MYPREFIX_DEBUG
        qDebug() << "startThread";
#endif
        thread->start();
    }
}

void MainWindow::showInvalidZipInTable(QSharedPtrListHFIS itemsPtr)
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "MainWindow::showInvalidZipInTable";
#endif
    QTableView *table = ui->tableView;
    if(table->model() != nullptr)
        table->model()->deleteLater();
    FileListTableModel *model = new FileListTableModel(itemsPtr, this);
    table->setModel(model);
    table->setSortingEnabled(true);
    table->sortByColumn(FileListTableModel::Column::fileName);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::closeEvent(QCloseEvent *event) {
#ifdef MYPREFIX_DEBUG
        qDebug() << "MainWindow::closeEvent";
#endif
    if (thread->isRunning()) {
        thread->requestInterruption();
        /*QMessageBox::warning(this,
                             "DirWizard",
                             tr("Background process already running!\nTry to exit after task would be complete."));
        event->ignore();*/
        thread->quit();
        thread->wait(250);
        event->accept();
    } else {
        event->accept();
    }
}

void MainWindow::setUiPushButtonsEnabled(bool flag)
{
    ui->pushButton_Calc_Hashes->setEnabled(flag);
    ui->pushButton_Check_Hashes->setEnabled(flag);
    ui->pushButton_Check_Zip->setEnabled(flag);
    ui->pushButton_Compare_Folders->setEnabled(flag);
    ui->pushButton_Duplicate_Search->setEnabled(flag);
    ui->pushButton_Remove_Checked->setEnabled(flag);
    ui->pushButton_Save_From_Table->setEnabled(flag);
}

// Drag Drop START
void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
    QMainWindow::dragEnterEvent(e);
}

void MainWindow::dropEvent(QDropEvent *e)
{
    #ifdef MYPREFIX_DEBUG
    for(const QString &format: e->mimeData()->formats()){
        qDebug()<<format<<"\n";
    }
    #endif
    // Check destination widget
    const QRect widgetRect = ui->listWidget->geometry();
    if(!widgetRect.contains(e->pos()))
        return;
    // Add file/folder
    for(const QUrl &url: e->mimeData()->urls()) {
        addToDirListWidget(url.toLocalFile());
    }
}
// Drag Drop END

// Custom popup menu for Table View
void MainWindow::createCustomPopupMenuForTableView(const QPoint &pos)
{
    const QTableView *table = ui->tableView;
    QMenu *menu = new QMenu(this);
    QAction *openFolder = new QAction(tr("Open Folder with selected file"), this);
    connect(openFolder, &QAction::triggered, [=](){
        if(table->model() == nullptr) return;
        const QModelIndexList indexList = table->selectionModel()->selectedIndexes();
        QString dir;
        const BaseTableModel *model = qobject_cast<BaseTableModel *>(table->model());
        foreach (QModelIndex index, indexList) {
            dir = model->getFileName(index);
        }
        QStringList commands;
        #if defined(Q_OS_WIN)
        commands<< "explorer.exe /select,\"" << QDir::toNativeSeparators(dir) <<"\"";
        QProcess::startDetached(commands.join(""));
        #elif defined(Q_OS_MAC)
        QStringList scriptArgs;
        scriptArgs << QLatin1String("-e")
                   << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
                      .arg(dir);
        QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
        scriptArgs.clear();
        scriptArgs << QLatin1String("-e")
                   << QLatin1String("tell application \"Finder\" to activate");
        QProcess::execute("/usr/bin/osascript", scriptArgs);
        #endif
    });
    menu->addAction(openFolder);
    menu->popup(table->viewport()->mapToGlobal(pos));
}
