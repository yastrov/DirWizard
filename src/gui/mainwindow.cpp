#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <limits>

namespace CONSTANTS {
    //const quint64 MAX_INT = static_cast<qint64>(std::numeric_limits<int>::max());
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dirNameForFolderDialog(QDir::current().dirName()),
    tableViewCustomPopupMenu(nullptr),
    thread(new QThread(this))
{
    ui->setupUi(this);
    setWindowTitle(qApp->applicationName());
    initOnceGui();
    initOnceConnectSlots();
#ifdef MYPREFIX_DEBUG
    qDebug()<<"before MainWindow::loadSettings";
#endif
    loadSettings();
}

void MainWindow::initOnceGui()
{
    initDirsListWidget();
    initHashComboBoxWidget();
    initTableWidget();
    // Drag and Drop
    setAcceptDrops(true);
    // Progress Bar
    ui->progressBar->setVisible(false);
    // Win Extras
#if defined(USE_WIN_EXTRAS) && defined(Q_OS_WIN)
    buttonWinExtra = new QWinTaskbarButton(this);
    buttonWinExtra->setWindow(windowHandle());
//    buttonWinExtra->setOverlayIcon(QIcon(":/loading.png"));
    progressWinExtra = buttonWinExtra->progress();
    progressWinExtra->setVisible(false);
#endif
#if defined(Q_OS_WIN)
    QAction *actionRemoveToTrash_checked = new QAction(ui->menuWork_with_results);
    actionRemoveToTrash_checked->setText(tr("Remove checked elements to system Trash (Recycle Bin)."));
    actionRemoveToTrash_checked->setToolTip(tr("Remove checked elements to system Trash (Recycle Bin)."));
    actionRemoveToTrash_checked->setStatusTip(tr("Remove checked elements to system Trash (Recycle Bin)."));
    ui->menuWork_with_results->insertAction(ui->actionRemove_checked, actionRemoveToTrash_checked);
    connect(actionRemoveToTrash_checked, &QAction::triggered, this, [this](bool checked){
        Q_UNUSED(checked)
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                  qApp->applicationName(),
                                                                  tr("Are you sure to remove checked items?"),
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
        if(ui->tableView->model() != nullptr) {
            qobject_cast<BaseTableModel*>(ui->tableView->model())->removeCheckedToTrashFunc();
        }
    });
#endif
}

void MainWindow::initOnceConnectSlots()
{
    QObject::connect(ui->actionAbout, &QAction::triggered, this, [this](bool checked){
        Q_UNUSED(checked)
        QMessageBox::about(this,
                           tr("About %1").arg(qApp->applicationName()),
                           tr("<h2>%1</h2>"
                              "<p>Written by Yuriy (Yuri) Astrov<br/>"
                              "Based on Qt 5<br/>"
                              "Licensed by GPLv2<br/>"
                              "Version: %2<br/>"
                              "URL: <a href='%3'>%3</a></p>").arg(qApp->applicationName(), APP_VERSION, APP_URL));
    });
    connect(ui->actionStart_Dupl_Find, &QAction::triggered, this, &MainWindow::on_pushButton_Duplicate_Search_clicked);
    connect(ui->actionCompare_Folders, &QAction::triggered, this, &MainWindow::on_pushButton_Compare_Folders_clicked);
    connect(ui->actionCheck_ZIP, &QAction::triggered, this, &MainWindow::on_pushButton_Check_Zip_clicked);
    connect(ui->actionCalc_Hashes, &QAction::triggered, this, &MainWindow::on_pushButton_Calc_Hashes_clicked);
    connect(ui->actionCheck_Hashes, &QAction::triggered, this, &MainWindow::on_pushButton_Check_Hashes_clicked);

    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::on_pushButton_Save_From_Table_clicked);
    connect(ui->actionOpen, &QAction::triggered, this, [this]{
        QTableView * const table = ui->tableView;
        const QString filters_all = "Text files (*.txt)";
        QString filter = "Text files (*.txt)";
        const QString fileName = QFileDialog::getOpenFileName(this,
                                                            tr("Open File"),
                                             QDir::homePath(),
                                             filters_all,
                                             &filter, QFileDialog::DontUseNativeDialog);
            if(!fileName.isNull() && !fileName.isEmpty())
            {
                if(table->model() != nullptr) {
                    table->model()->deleteLater();
                }
                DuplicatesTableModel *model = new DuplicatesTableModel(this);
                model->loadFromFileFunc(fileName);
                table->setModel(model);
                table->sortByColumn(DuplicatesTableModel::Column::groupId);
                table->resizeColumnsToContents();
            }
    });
    connect(ui->actionRemove_checked, &QAction::triggered, this, &MainWindow::on_pushButton_Remove_Checked_clicked);
    connect(ui->actionSelect_files_in_concrete_folder, &QAction::triggered, this, [this](){
        const QString dirName = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
                                                     dirNameForFolderDialog,
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
        if(!dirName.isEmpty() && !dirName.isNull())
        {
            QTableView * const table = ui->tableView;
            if(table->model()==nullptr) {
                return;
            }
            qobject_cast<BaseTableModel*>(table->model())->selectFilesInFolder(dirName);
        }
    });
    connect(ui->actionClear_all_flags, &QAction::triggered, this, [this](){
        if(ui->tableView->model()==nullptr) {
            return;
        }
        qobject_cast<BaseTableModel*>(ui->tableView->model())->unselectAll();
    });
    connect(ui->actionFind_Empty_folders, &QAction::triggered, this, [this](){
        if (thread->isRunning()) {
            thread->wait();
        }
        //ClearItemsResultStore();
        QList<QDir> dirs = getElementsFromDirsListWidget();
        if(!dirs.isEmpty() && !thread->isRunning())
        {
            EmptyDirFinder *worker = new EmptyDirFinder(nullptr);
            worker->setQDir(dirs);
            if(useFilters()) worker->setFilters(fileFilters);
            callBeforeBackgrowndWorkerStarted();
            worker->moveToThread(thread);
            //connect(thread, SIGNAL(started()), worker, SLOT(process()));
            QObject::connect(thread, &QThread::started, worker, &EmptyDirFinder::process);
            //connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
            QObject::connect(worker, &EmptyDirFinder::finished, thread, &QThread::quit);
            QObject::connect(thread, &QThread::finished, worker, &EmptyDirFinder::deleteLater);//From Off documentation
            QObject::connect(worker, &EmptyDirFinder::finished, this, &MainWindow::finishedThread);
            QObject::connect(worker, &EmptyDirFinder::finishedWData, this, [this](QSharedPtrListHFIS itemsPtr){
#ifdef MYPREFIX_DEBUG
                qDebug() << "&EmptyDirFinder::finishedWData received data";
#endif
                QTableView *const table = ui->tableView;
                if(table->model() != nullptr)
                    table->model()->deleteLater();
                EmptyFolderTableModel *model = new EmptyFolderTableModel(itemsPtr, this);
                table->setModel(model);
                table->sortByColumn(EmptyFolderTableModel::Column::fileName);
                table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
                table->resizeColumnsToContents();
                table->horizontalHeader()->setStretchLastSection(true);
            });
            connect(worker, &EmptyDirFinder::sayTotalFiles, this, &MainWindow::maximumFilesFoProgressReceived);
            connect(worker, &EmptyDirFinder::currentProcessedFiles, this, &MainWindow::currentProcessedFilesForProgressReceived);
#ifdef MYPREFIX_DEBUG
            qDebug() << "startThread";
#endif
            thread->start();
            setUiPushButtonsEnabled(false);
        }
    });
    connect(ui->actionAbout_Qt, &QAction::triggered, this, [this](){ QMessageBox::aboutQt(this); });
    QObject::connect(ui->actionSet_Font, &QAction::triggered, this, [this](bool checked){
        Q_UNUSED(checked)
        bool ok;
        int fontSize;
        QString fontFamily;
        _settingsHelper.loadFont(fontSize, fontFamily);
        QFont font = QFontDialog::getFont(
                        &ok, QFont(fontFamily, fontSize), this);
        if (ok) {
            _settingsHelper.saveFont(font.pointSize(), font.family());
            // Try to move it to helper failed.
            this->setStyleSheet(QString("font-size:%1pt;font-family:%2;").arg(fontSize).arg(fontFamily));
        } else {
            ;
        }
    });
}

void MainWindow::showEvent(QShowEvent *e)
{
#if defined(USE_WIN_EXTRAS) && defined(Q_OS_WIN)
    buttonWinExtra->setWindow(windowHandle());
#endif

    e->accept();
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
    const int count = ui->listWidget->count();
    QListWidgetItem *item;
    QList<QDir> dirList;
    dirList.reserve(count);
    QDir dir;

    QListWidget *list = ui->listWidget;
    for(int i=0; i < count; ++i)
    {
        item = list->item(i);
        dir = QDir(item->data(Qt::UserRole).toString());
        /* Now it works in Backgrownd workers
        dir.setNameFilters(qsl);
        */
        dirList.append(dir);
    }
    return dirList;
}

void MainWindow::addToDirListWidget(const QString &dirName)
{
    QAbstractItemModel *model = ui->listWidget->model();
    const int rows = model->rowCount();
    QString str;
    for(int i=0; i<rows; ++i) {
        str = model->data(model->index(i, 0), Qt::UserRole).toString();
        if(str == dirName)
            return;
    }
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

void MainWindow::addItemToComboBox(const QString &text, int data)
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
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table->horizontalHeader()->setSectionsMovable(true);
    table->setSortingEnabled(true);
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
    table->sortByColumn(DuplicatesTableModel::Column::groupId);
    table->resizeColumnsToContents();
}
// END

void MainWindow::callBeforeBackgrowndWorkerStarted()
{
    ui->progressBar->setVisible(true);
    ui->progressBar->setValue(ui->progressBar->minimum());
    ui->progressBar->setStatusTip("");
#if defined(USE_WIN_EXTRAS) && defined(Q_OS_WIN)
    if(progressWinExtra != nullptr) {
        progressWinExtra->setMinimum(0);
        progressWinExtra->setVisible(true);
    }
#endif

}

// Duplicate Files Search
void MainWindow::startDuplicateSearchInBackground()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "startDuplicateSearchInBackground";
#endif
    if (thread->isRunning()) {
        thread->wait();
    }
    QList<QDir> dirs = getElementsFromDirsListWidget();
    if(!dirs.isEmpty() && !thread->isRunning())
    {
        DuplicateFinder *worker = new DuplicateFinder(getCurrentHashAlgo(), nullptr);
        worker->setQDir(dirs);
        if(useFilters()) worker->setFilters(fileFilters);
        callBeforeBackgrowndWorkerStarted();
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

        connect(worker, &DuplicateFinder::sayTotalFiles, this, &MainWindow::maximumFilesFoProgressReceived);
        connect(worker, &DuplicateFinder::currentProcessedFiles, this, &MainWindow::currentProcessedFilesForProgressReceived);
#ifdef MYPREFIX_DEBUG
        qDebug() << "startThread";
#endif
        thread->start();
        setUiPushButtonsEnabled(false);
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
    table->sortByColumn(FileListTableModel::Column::fileName);
}

void MainWindow::startComparingFoldersInBackground()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "startComparingFoldersInBackground";
#endif
    if (thread->isRunning()) {
        thread->wait();
    }
    QList<QDir> dirs = getElementsFromDirsListWidget();
    if(!dirs.isEmpty() && !thread->isRunning())
    {
        DirComparator *worker = new DirComparator(getCurrentHashAlgo(), nullptr);
        worker->setQDir(dirs);
        if(useFilters()) worker->setFilters(fileFilters);
        callBeforeBackgrowndWorkerStarted();
        worker->moveToThread(thread);
        //connect(thread, SIGNAL(started()), worker, SLOT(process()));
        QObject::connect(thread, &QThread::started, worker, &DirComparator::process);
        //connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
        QObject::connect(worker, &DirComparator::finished, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, worker, &DirComparator::deleteLater);//From Off documentation
        QObject::connect(worker, &DirComparator::finished, this, &MainWindow::finishedThread);
        //connect(worker, SIGNAL(finishedWData(QList<HashFileInfoStruct> *)), this, SLOT(compareFoldersComplete(QList<HashFileInfoStruct> *)));
        QObject::connect(worker, &DirComparator::finishedWData, this, &MainWindow::showUniqFilesInTable);
        connect(worker, &DirComparator::sayTotalFiles, this, &MainWindow::maximumFilesFoProgressReceived);
        connect(worker, &DirComparator::currentProcessedFiles, this, &MainWindow::currentProcessedFilesForProgressReceived);
#ifdef MYPREFIX_DEBUG
        qDebug() << "startThread";
#endif
        thread->start();
        setUiPushButtonsEnabled(false);
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
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              qApp->applicationName(),
                                                              tr("Are you sure to remove checked items?"),
                                                              QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }
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
    startDuplicateSearchInBackground();
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

void MainWindow::finishedThread()
{
#if defined(USE_WIN_EXTRAS) && defined(Q_OS_WIN)
    if(progressWinExtra != nullptr)
        progressWinExtra->setVisible(false);
#endif
    ui->progressBar->setVisible(false);
    setUiPushButtonsEnabled(true);
    QMessageBox::information(this,
                             qApp->applicationName(),
                             tr("Task completed successfully!"));
}


void MainWindow::on_pushButton_Compare_Folders_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << "\n";
    qDebug() << "QThread: " << thread << "\n";
#endif
    startComparingFoldersInBackground();
}

void MainWindow::on_pushButton_Save_From_Table_clicked()
{
    QString filter = "Text files (*.txt)";
    QString cfilter = "Text files (*.txt)";
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save File"),
                                     QDir::homePath()+"/results.txt",
                                     filter,
                                     &cfilter);
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
                                                                      windowTitle(),
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
    if (thread->isRunning()) {
        thread->wait();
    }
    QList<QDir> dirs = getElementsFromDirsListWidget();
    if(!dirs.isEmpty() && !thread->isRunning())
    {
        CalcAndSaveHash *worker = new CalcAndSaveHash(getCurrentHashAlgo(), nullptr);

        worker->setQDir(dirs);
        if(useFilters()) worker->setFilters(fileFilters);
        callBeforeBackgrowndWorkerStarted();
        worker->moveToThread(thread);
        QObject::connect(thread, &QThread::started, worker, &CalcAndSaveHash::process);
        QObject::connect(worker, &CalcAndSaveHash::finished, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, worker, &CalcAndSaveHash::deleteLater);//From Off documentation
        QObject::connect(worker, &CalcAndSaveHash::finished, this, &MainWindow::finishedThread);
        connect(worker, &CalcAndSaveHash::sayTotalFiles, this, &MainWindow::maximumFilesFoProgressReceived);
        connect(worker, &CalcAndSaveHash::currentProcessedFiles, this, &MainWindow::currentProcessedFilesForProgressReceived);
#ifdef MYPREFIX_DEBUG
        qDebug() << "startThread";
#endif
        thread->start();
        setUiPushButtonsEnabled(false);
    }
}

void MainWindow::on_pushButton_Calc_Hashes_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << "\n";
    qDebug() << "QThread: " << thread << "\n";
#endif
    startCalcHashesInBackground();
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
    startCheckHashesInBackground();
}

void MainWindow::startCheckHashesInBackground()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "startCheckHashesInBackground";
#endif
    if (thread->isRunning()) {
        thread->wait();
    }
    QList<QDir> dirs = getElementsFromDirsListWidget();
    if(!dirs.isEmpty() && !thread->isRunning())
    {
        LoadAndCheckHash *worker = new LoadAndCheckHash(getCurrentHashAlgo(), nullptr);

        worker->setQDir(dirs);
        if(useFilters()) worker->setFilters(fileFilters);
        callBeforeBackgrowndWorkerStarted();
        worker->moveToThread(thread);
        QObject::connect(thread, &QThread::started, worker, &LoadAndCheckHash::process);
        QObject::connect(worker, &LoadAndCheckHash::finished, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, worker, &LoadAndCheckHash::deleteLater);//From Off documentation
        QObject::connect(worker, &LoadAndCheckHash::finished, this, &MainWindow::finishedThread);
        QObject::connect(worker, &LoadAndCheckHash::finishedWData, this, &MainWindow::showInvalidHashFilesInTable);
        connect(worker, &LoadAndCheckHash::sayTotalFiles, this, &MainWindow::maximumFilesFoProgressReceived);
        connect(worker, &LoadAndCheckHash::currentProcessedFiles, this, &MainWindow::currentProcessedFilesForProgressReceived);
#ifdef MYPREFIX_DEBUG
        qDebug() << "startThread";
#endif
        thread->start();
        setUiPushButtonsEnabled(false);
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
    table->sortByColumn(FileListTableModel::Column::fileName);
}

void MainWindow::on_pushButton_Check_Zip_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "MainWindow::on_pushButton_Check_Zip_clicked";
#endif
    startCheckZipsInBackground();
}

void MainWindow::startCheckZipsInBackground()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "MainWindow::startCheckZipsInBackground";
#endif
    if (thread->isRunning()) {
        thread->wait();
    }
    QList<QDir> dirs = getElementsFromDirsListWidget();
    if(!dirs.isEmpty() && !thread->isRunning())
    {
        ZipWalkChecker *worker = new ZipWalkChecker(nullptr);
        worker->setQDir(dirs);
        callBeforeBackgrowndWorkerStarted();
        worker->moveToThread(thread);
        QObject::connect(thread, &QThread::started, worker, &ZipWalkChecker::process);
        QObject::connect(worker, &ZipWalkChecker::finished, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, worker, &ZipWalkChecker::deleteLater);//From Off documentation
        QObject::connect(worker, &ZipWalkChecker::finished, this, &MainWindow::finishedThread);
        QObject::connect(worker, &ZipWalkChecker::finishedWData, this, &MainWindow::showInvalidZipInTable);
        connect(worker, &ZipWalkChecker::sayTotalFiles, this, &MainWindow::maximumFilesFoProgressReceived);
        connect(worker, &ZipWalkChecker::currentProcessedFiles, this, &MainWindow::currentProcessedFilesForProgressReceived);
#ifdef MYPREFIX_DEBUG
        qDebug() << "startThread";
#endif
        thread->start();
        setUiPushButtonsEnabled(false);
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
    table->sortByColumn(FileListTableModel::Column::fileName);
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
        storeSettings();
        event->accept();
    }
}

void MainWindow::setUiPushButtonsEnabled(bool flag)
{
    ui->pushButton_Duplicate_Search->setEnabled(flag);
    ui->pushButton_Remove_Checked->setEnabled(flag);
    ui->pushButton_Save_From_Table->setEnabled(flag);
    ui->menuCommands->setEnabled(flag);
    ui->menuFile->setEnabled(flag);
    ui->menuWork_with_results->setEnabled(flag);
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

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

// Custom popup menu for Table View
void MainWindow::createCustomPopupMenuForTableView(const QPoint &pos)
{
    const QTableView *table = ui->tableView;
    if(!tableViewCustomPopupMenu) {
    tableViewCustomPopupMenu = new QMenu(this);
    QAction *openFolder = new QAction(tr("Open Folder with selected file"), this);
    connect(openFolder, &QAction::triggered, this, [this](){
        const QTableView *table = ui->tableView;
        if(table->model() == nullptr) return;
        const QModelIndexList indexList = table->selectionModel()->selectedIndexes();
        QString dir;
        const BaseTableModel *model = qobject_cast<BaseTableModel *>(table->model());
        for(const QModelIndex &index: indexList) {
            dir = model->getFileName(index);
        }
        QStringList commands;
        #if defined(Q_OS_WIN)
        commands<< "explorer.exe /select, \"" << QDir::toNativeSeparators(dir) <<"\"";
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
    tableViewCustomPopupMenu->addAction(openFolder);
    }
    tableViewCustomPopupMenu->popup(table->viewport()->mapToGlobal(pos));
}

bool MainWindow::useFilters() const
{
    return ui->useFiltersBtn->isChecked();
}

void MainWindow::on_setFiltersBtn_clicked()
{
    FiltersDialog f;
    f.setActiveFilters(fileFilters);
    f.setStyleSheet(this->styleSheet());
    if(f.exec() == QDialog::Accepted){
        fileFilters = f.getActiveFilters();
    }
}

void MainWindow::maximumFilesFoProgressReceived(q_coll_s_t count)
{
    ui->progressBar->setMaximum(count);
#if defined(USE_WIN_EXTRAS) && defined(Q_OS_WIN)
    progressWinExtra->setMaximum(count);
#endif
}

void MainWindow::currentProcessedFilesForProgressReceived(q_coll_s_t count)
{
    ui->progressBar->setValue(count);
#if defined(USE_WIN_EXTRAS) && defined(Q_OS_WIN)
    progressWinExtra->setValue(count);
#endif
    const QString s = QString::number(count) + " / " + QString::number(ui->progressBar->maximum());
    ui->progressBar->setStatusTip(s);
    ui->statusBar->showMessage(s);
}

void MainWindow::storeSettings()
{
#ifdef MYPREFIX_DEBUG
    qDebug()<<"MainWindow::storeSettings";
#endif
    _settingsHelper.saveFileExtFilters(fileFilters);
    _settingsHelper.saveHashIndex(ui->comboBox->currentIndex());
    _settingsHelper.saveMainWindowPosition(size(), pos());
}

void MainWindow::loadSettings()
{
#ifdef MYPREFIX_DEBUG
    qDebug()<<"MainWindow::loadSettings";
#endif
    fileFilters = _settingsHelper.loadFileExtFilters();
    this->setStyleSheet(_settingsHelper.loadFontStyleSheet());
    ui->comboBox->setCurrentIndex(_settingsHelper.loadHashIndex());
    // Load MainWindow position and size
    QSize size;
    QPoint pos;
    _settingsHelper.loadMainWindowPosition(size, pos);
    resize(size);
    move(pos);
}
