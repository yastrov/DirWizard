#include "zipwalkchecker.h"

ZipWalkChecker::ZipWalkChecker(QObject *parent):
    DirWalker(parent),
    itemsList(new QList<HashFileInfoStruct>())
{

}

ZipWalkChecker::~ZipWalkChecker()
{

}

void ZipWalkChecker::process()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "ZipWalkChecker::process";
#endif
    QStringList filters;
    QDir current;
    QMutableListIterator<QDir> it(rootDirs);
    filters << "*.zip";
    while(it.hasNext())
    {
        current = it.next();
        current.setNameFilters(filters);
        it.setValue(current);
    }
    DirWalker::processFilesRecursively(rootDirs);
    emit currentProcessedFiles(total_files);
    emit finishedWData(itemsList);
    emit finished();
}

void ZipWalkChecker::processFile(const QString &fileName)
{
    if(fileName.isEmpty() || fileName.isNull())
        return;
#ifdef MYPREFIX_DEBUG
    qDebug() << "ZipWalkChecker::processFile";
#endif
    QFileInfo fInfo(fileName);
    if( fInfo.isFile() && fInfo.isReadable() )
    {
        QuaZip qzip(fileName);
        if(qzip.open(QuaZip::mdUnzip))
        {
            qzip.goToFirstFile();
        }
        if(qzip.getZipError() != UNZ_OK)
        {
            HashFileInfoStruct fs;
            fs.fileName = fileName;
            fs.size = fInfo.size();
            fs.checked = true;
            itemsList->append(fs);
        }
        if(qzip.isOpen())
            qzip.close();
    }
    ++processed_files;
    if(processed_files %10 == 0)
        emit currentProcessedFiles(processed_files);
}

void ZipWalkChecker::processFilesRecursively(const QDir &rootDir) {
    // Calc number of all files
    QDirIterator it0(rootDir, QDirIterator::Subdirectories);
    while(it0.hasNext() && !stopped) {
        it0.next();
        ++total_files;
        if(QThread::currentThread()->isInterruptionRequested())
            stopped=true;
    }
    emit sayTotalFiles(total_files);
    QDirIterator it(rootDir, QDirIterator::Subdirectories);
    while(it.hasNext() && !stopped) {
        processFile(it.next());
        if(QThread::currentThread()->isInterruptionRequested())
        stopped=true;
    }
    if(stopped)
    {
        emit finished();
    }
}

