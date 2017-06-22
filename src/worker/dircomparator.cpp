#include "dircomparator.h"

DirComparator::DirComparator(QCryptographicHash::Algorithm hash, QObject *parent):
    HashDirWalker(hash, parent),
    resultCount(0)
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DirComparator::constructor";
#endif
}

DirComparator::~DirComparator()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DirComparator::~DirComparator";
#endif
}

void DirComparator::process()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::process";
#endif
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    calcTotalFiles();
    emit sayTotalFiles(total_files);
    DirWalker::processFilesRecursively();
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    clearNoDuplicatedHashes();
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::process::reduceToResult";
#endif
    reduceToResult();
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::process::emit";
    qDebug() << "Num of uniq: "<< result.data()->count();
#endif
    emit currentProcessedFiles(total_files);
    emit finishedWData(result);
    emit finished();
}

void DirComparator::processFile(const QString &fileName)
{
    if(fileName.isEmpty() || fileName.isNull())
        return;
    #ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::processFile";
#endif
    QFileInfo fInfo(fileName);
    if( fInfo.isFile() && fInfo.isReadable() )
    {
        QByteArray qb = fileChecksum(fileName, hashAlgo);
        if(!qb.isNull() && !qb.isEmpty())
        {
            HashFileInfoStruct st;
            st.fileName = fileName;
            st.size = fInfo.size();
            st.checked = false;
            st.hash = QString(qb);
            hashByHash.insert(st.hash, st);
        }
    }
    ++processed_files;
    if(processed_files %10 == 0)
        emit currentProcessedFiles(processed_files);
}

void DirComparator::clearNoDuplicatedHashes()
{
    #ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::clearNoDuplicatedHashes";
#endif
    int count;
    for(const QString &key: hashByHash.uniqueKeys())
    {
       count = hashByHash.count(key);
       if(count > 1) {
           hashByHash.remove(key);
       } else {
           resultCount += count;
       }
    }
}

void DirComparator::reduceToResult()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DirComparator::reduceToResult";
#endif
    result = QSharedPtrListHFIS(new QList<HashFileInfoStruct>());
#ifdef MYPREFIX_DEBUG
    qDebug() << "DirComparator::reduceToResult:: reserve memory for result";
#endif
    result.data()->reserve(resultCount);
    HashFileInfoStruct s;
    QList<HashFileInfoStruct> * const list = result.data();
    int groupId = 0;
    QList<HashFileInfoStruct> values = hashByHash.values();
    QMutableListIterator<HashFileInfoStruct> it(values);
    while(it.hasNext()){
        s = it.next();
        s.groupID = groupId;
        s.checked = false;
        list->append(std::move(s));
        ++groupId;
    }
    hashByHash.clear();
#ifdef MYPREFIX_DEBUG
    qDebug() << "DirComparator::reduceToResult:: return result";
#endif
}

