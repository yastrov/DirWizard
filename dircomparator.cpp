#include "dircomparator.h"

DirComparator::DirComparator(QCryptographicHash::Algorithm hash, QObject *parent):
    HashDirWalker(hash, parent),
    //hashAlgo(hash),
    resultCount(0)
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DirComparator::constructor";
#endif
}

void DirComparator::process()
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
            if(hashByHash.contains(st.hash))
            {
                hashByHash[st.hash].append(st);
            }
            else
            {
                QVector<HashFileInfoStruct> v(SizeForInnerVector);
                v.append(st);
                hashByHash[st.hash] = v;
            }
        }
    }
}

void DirComparator::clearNoDuplicatedHashes()
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
       if(count > 1)
       {
           hashByHash.remove(key);
       }
       else
       {
           resultCount += count;
       }
    }
}

QList<HashFileInfoStruct> * DirComparator::reduceToResult()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DirComparator::reduceToResult";
#endif
    QList<HashFileInfoStruct> *result = new QList<HashFileInfoStruct>();
#ifdef MYPREFIX_DEBUG
    qDebug() << "DirComparator::reduceToResult:: reserve memory for result";
#endif
    result->reserve(resultCount);

    QList<QString> keys = hashByHash.keys();
    QListIterator<QString> it(keys);
    QString key;
    QVector<HashFileInfoStruct> vect;
    while(it.hasNext())
    {
       key = it.next();
       vect = hashByHash[key];
       QMutableVectorIterator<HashFileInfoStruct> vIt( vect);
       while(vIt.hasNext())
       {
           result->append(vIt.next());
       }
    }
    hashByHash.clear();
#ifdef MYPREFIX_DEBUG
    qDebug() << "DirComparator::reduceToResult:: return result";
#endif
    return result;
}
