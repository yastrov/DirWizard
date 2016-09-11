#include "duplicatefinder.h"
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif

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
    processFilesRecursively(rootDirs);
    emit sayTotalFiles(total_files);
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    clearNoDuplicatedSize();
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    makeHashByHashes();
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
    qDebug() << "Num of duplacates: "<<result.data()->count();
#endif
    emit currentProcessedFiles(total_files);
    emit finishedWData(result);
    emit finished();
}

void DuplicateFinder::processFile(const QString &fileName)
{
    if(fileName.isEmpty() || fileName.isNull())
        return;
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::processFile";
#endif
    QFileInfo fInfo(fileName);
    if(fInfo.isFile() && fInfo.isReadable())
    {
        QByteArray qb = fileChecksum(fileName, hashAlgo);
        if(!qb.isNull() && !qb.isEmpty())
        {
            HashFileInfoStruct st;
            st.fileName = fileName;
            st.size = fInfo.size();
            st.checked = false;
#ifdef HASH_OV_VECT
            if(hashBySize.contains(st.size))
            {
                hashBySize[st.size].append(st);
            }
            else
            {
                QVector<HashFileInfoStruct> v(SizeForInnerVector);
                v.append(st);
                hashBySize[st.size] = v;
            }
#else
            hashBySize.insert(st.size, st);
#endif
        }
    }
}

void DuplicateFinder::clearNoDuplicatedSize()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::clearNoDuplicatedSize";
#endif
#ifdef HASH_OV_VECT
    qint64 key;
    QVector<HashFileInfoStruct> vect;
    QList<qint64> keys = hashBySize.keys();
    QListIterator<qint64> it(keys);
    while(it.hasNext())
    {
        key = it.next();
        vect = hashBySize[key];
        if(vect.count() < 2)
        {
            hashBySize.remove(key);
        }
    }
    if(hashBySize.contains(0))
        hashBySize.remove(0);
#else
    QListIterator<qint64> it(hashBySize.keys());
    while(it.hasNext())
    {
       const qint64 &key = it.next();
       if(hashBySize.count(key) < 2) {
           ++processed_files;
           hashBySize.remove(key);
       }
    }
#endif
}

void DuplicateFinder::makeHashByHashes()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::makeHashByHashes";
#endif
    HashFileInfoStruct s;
#ifdef HASH_OV_VECT
    QList<QVector<HashFileInfoStruct> > qList = hashBySize.values();
    QMutableListIterator<QVector<HashFileInfoStruct> > qListIt (qList);
    while(qListIt.hasNext())
    {
        QVector<HashFileInfoStruct> qV = qListIt.next();
        QMutableVectorIterator<HashFileInfoStruct> qVIt (qV);
        while(qVIt.hasNext())
        {
            s = qVIt.next();
            QByteArray qb = fileChecksum(s.fileName, hashAlgo);
            if(!qb.isNull() && !qb.isEmpty())
            {
                s.hash = QString(qb);
                if(!s.hash.isEmpty() && s.size > 0)
                {
                    if(hashByHash.contains(s.hash))
                    {
                        hashByHash[s.hash].append(s);
                    }
                    else
                    {
                        QVector<HashFileInfoStruct> vect2(SizeForInnerVector);
                        vect2.append(s);
                        hashByHash[s.hash] = vect2;
                    }
                }
            }
        }
    }
#else
    QList<HashFileInfoStruct> qList = hashBySize.values();
    QMutableListIterator<HashFileInfoStruct> qListIt(qList);
    while(qListIt.hasNext())
    {
       s = qListIt.next();
       QByteArray qb = fileChecksum(s.fileName, hashAlgo);
       if(!qb.isNull() && !qb.isEmpty())
       {
           s.hash = QString(qb);
           if(!s.hash.isEmpty() && s.size > 0)
           {
               hashByHash.insert(s.hash, s);
           }
       }
       ++processed_files;
       if(processed_files %10 == 0)
           emit currentProcessedFiles(processed_files);
    }
#endif
    hashBySize.clear();
}

void DuplicateFinder::clearNoDuplicatedHashes()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::clearNoDuplicatedHashes";
#endif
    QList<QString> keys = hashByHash.keys();
#ifdef HASH_OV_VECT
    QString key;
    QMutableListIterator<QString> it(keys);
    QVector<HashFileInfoStruct> vect;
    int count;
    while(it.hasNext())
    {
       key = it.next();
       vect = hashByHash[key];
       count = vect.count();
       if(count < 2)
       {
           hashByHash.remove(key);
       }
       else
       {
           resultCount += count;
       }
    }
#else
    QListIterator<QString> it(keys);
    while(it.hasNext())
    {
       const QString &key = it.next();
       int count = hashByHash.count(key);
       if(count < 2) {
           hashByHash.remove(key);
       } else {
           resultCount += count;
       }
    }
#endif
}

void DuplicateFinder::reduceToResult()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::reduceToResult";
#endif
    result = QSharedPtrListHFIS(new QList<HashFileInfoStruct>());
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::reduceToResult:: reserve memory for result";
#endif
    result.data()->reserve(resultCount);
    HashFileInfoStruct s;
    int groupId = 0;
    bool checked = false;
    QList<QString> keys = hashByHash.keys();
#ifdef HASH_OV_VECT
    QString key;
    QListIterator<QString> it(keys);
    QVector<HashFileInfoStruct> vect;
    while(it.hasNext())
    {
       key = it.next();
       vect = hashByHash[key];
       checked = false;
       QMutableVectorIterator<HashFileInfoStruct> vIt(vect);
       while(vIt.hasNext())
       {
           s = vIt.next();
           s.groupID = groupId;
           s.checked = checked;
           result.data()->append(std::move(s));
           checked = true;
       }
       ++groupId;
    }
#else
    QList<HashFileInfoStruct> * const list = result.data();
    QListIterator<QString> it(keys);
    while(it.hasNext())
    {
        const QString &key = it.next();
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
#endif
    hashByHash.clear();
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::reduceToResult:: return result";
#endif
}
