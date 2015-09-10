#include "duplicatefinder.h"

DuplicateFinder::DuplicateFinder(QCryptographicHash::Algorithm hash, QObject *parent) :
    HashDirWalker(hash, parent),
    resultCount(0)
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::constructor";
#endif
}

void DuplicateFinder::process()
{
    #ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::process";
#endif
    if(stopped)
    {
        emit finished();
        return;
    }
    processFilesRecursively(rootDirs);
    if(stopped)
    {
        emit finished();
        return;
    }
    clearNoDuplicatedSize();
    if(stopped)
    {
        emit finished();
        return;
    }
    makeHashByHashes();
    if(stopped)
    {
        emit finished();
        return;
    }
    clearNoDuplicatedHashes();
    if(stopped)
    {
        emit finished();
        return;
    }
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::process::reduceToResult";
#endif
    QList<HashFileInfoStruct> *items = reduceToResult();
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::process::emit";
#endif
    emit finishedWData(items);
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
        }
    }
}

void DuplicateFinder::clearNoDuplicatedSize()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::clearNoDuplicatedSize";
#endif
    QVector<HashFileInfoStruct> vect;
    QList<qint64> keys = hashBySize.keys();
    qint64 key;
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
}

void DuplicateFinder::makeHashByHashes()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::makeHashByHashes";
#endif
    QList<QVector<HashFileInfoStruct> > qList = hashBySize.values();
    QMutableListIterator<QVector<HashFileInfoStruct> > qListIt (qList);
    HashFileInfoStruct s;
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
    hashBySize.clear();
}

void DuplicateFinder::clearNoDuplicatedHashes()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::clearNoDuplicatedHashes";
#endif
    QList<QString> keys = hashByHash.keys();
    QMutableListIterator<QString> it(keys);
    QString key;
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
}

QList<HashFileInfoStruct> * DuplicateFinder::reduceToResult()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::reduceToResult";
#endif
    QList<HashFileInfoStruct> *result = new QList<HashFileInfoStruct>();
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::reduceToResult:: reserve memory for result";
#endif
    result->reserve(resultCount);

    QList<QString> keys = hashByHash.keys();
    QListIterator<QString> it(keys);
    QString key;
    QVector<HashFileInfoStruct> vect;
    HashFileInfoStruct s;
    int groupId = 0;
    bool checked = false;
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
           result->append(s);
           checked = true;
       }
       groupId++;
    }
    hashByHash.clear();
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::reduceToResult:: return result";
#endif
    return result;
}
