#include "emptydirfinder.h"

EmptyDirFinder::EmptyDirFinder(QObject *parent):
    DirWalker(parent)
{

}

void EmptyDirFinder::process()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::process";
#endif
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    QDir current;
    QMutableListIterator<QDir> it(rootDirs);
    while(it.hasNext())
    {
        current = it.next();
        current.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
        it.setValue(current);
    }
    calcTotalFiles();
    emit sayTotalFiles(total_files);
    processFilesRecursively();
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    result = QSharedPtrListHFIS(new QList<HashFileInfoStruct>());
    result.data()->reserve(processed_files);
    while (!tempList.isEmpty()) {
         result->append(std::move(tempList.takeFirst()));
    }
    emit currentProcessedFiles(total_files);
    emit finishedWData(result);
    emit finished();
}

void EmptyDirFinder::processFile(const QString &fileName)
{
    if(fileName.isEmpty() || fileName.isNull())
        return;
#ifdef MYPREFIX_DEBUG
    qDebug() << "EmptyDirFinder::processFile "<<fileName;
#endif
    static const QStringList nameFilters("*.*");
    QFileInfo fInfo(fileName);
    if(fInfo.isDir())
    {
        QDir dir(fileName);
        dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
        dir.setNameFilters(nameFilters);
        if(dir.entryList().size() == 0)
        {
            HashFileInfoStruct st;
            st.fileName = fileName;
            st.checked = true;
            tempList.append(std::move(st));
            ++processed_files;
            if(processed_files % SAY_PROGRESS_EVERY == 0)
                emit currentProcessedFiles(processed_files);
        }
    }
}
