#include "duplicatefinder.h"

template<typename container>
void removeUniqueKeysFromMultHash(container& hash)
{
    for(const auto& key: hash.uniqueKeys())
    {
       if(hash.count(key) < 2) {
           hash.remove(key);
       }
    }
}

DuplicateFinder::DuplicateFinder(QCryptographicHash::Algorithm hash, QObject *parent) :
    HashDirWalker(hash, parent),
    resultCount(0)
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::constructor";
#endif
}

DuplicateFinder::~DuplicateFinder()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::~DuplicateFinder";
#endif
}

void DuplicateFinder::process()
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
    hashBySize.reserve(total_files);
    processFilesRecursively();
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    const int before = hashBySize.size();
    removeUniqueKeysFromMultHash(hashBySize);
    processed_files +=  before - hashBySize.size();
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    hashByHash.reserve(hashBySize.size());
    makeHashByHashes();
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    removeUniqueKeysFromMultHash(hashByHash);
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    reduceToResult();
    emit currentProcessedFiles(total_files);
    emit finishedWData(result);
    emit finished();
}

void DuplicateFinder::processFile(const QString &fileName)
{
    if(fileName.isEmpty() || fileName.isNull())
        return;
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::processFile "<<fileName;
#endif
    QFileInfo fInfo(fileName);
    if(fInfo.isFile() && fInfo.isReadable())
    {
            HashFileInfoStruct st;
            st.fileName = fileName;
            st.size = fInfo.size();
            st.checked = false;
            hashBySize.insert(st.size, st);
    }
}


void DuplicateFinder::makeHashByHashes()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::makeHashByHashes";
#endif
    HashFileInfoStruct s;
    QList<HashFileInfoStruct> qList = hashBySize.values();
    QMutableListIterator<HashFileInfoStruct> qListIt(qList);
    QByteArray qb;
    while(qListIt.hasNext())
    {
       s = qListIt.next();
       qb = fileChecksum(s.fileName, hashAlgo);
       if(!qb.isNull() && !qb.isEmpty())
       {
           s.hash = QString(qb);
           if(!s.hash.isEmpty())
           {
               hashByHash.insert(s.hash, s);
           }
       }
       ++processed_files;
       if(processed_files % SAY_PROGRESS_EVERY == 0)
           emit currentProcessedFiles(processed_files);
    }
    hashBySize.clear();
}

void DuplicateFinder::reduceToResult()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::reduceToResult";
#endif
    result = QSharedPtrListHFIS(new QList<HashFileInfoStruct>());
    result.data()->reserve(hashByHash.size());
    HashFileInfoStruct s;
    int groupId = 0;
    bool checked = false;
    QList<HashFileInfoStruct> * const list = result.data();
    for(const QString &key: hashByHash.uniqueKeys())
    {
        checked = false;
        QList<HashFileInfoStruct> values = hashByHash.values(key);
        QMutableListIterator<HashFileInfoStruct> vIt(values);
        while(vIt.hasNext())
        {
            s = vIt.next();
            s.groupID = groupId;
            s.checked = checked;
            list->append(std::move(s));
            checked = true;
        }
        ++groupId;
    }
    hashByHash.clear();
}
