#ifndef DIRCOMPARATOR_H
#define DIRCOMPARATOR_H
#include <QObject>
#include <QLinkedList>
#include <QVector>
#include <QHash>
#include <QMutableListIterator>
#include <QLinkedListIterator>
#include <QListIterator>
#include <QList>
#include <QMutableVectorIterator>
#include <QVectorIterator>
#include <QFileInfo>
#include <QCryptographicHash>
#include <hashfileinfostruct.h>
#include <hashdirwalker.h>
#include <QDebug>

#define SizeForInnerVector 0

class DirComparator : public HashDirWalker
{
     Q_OBJECT
public:
    DirComparator(QCryptographicHash::Algorithm hash, QObject *parent = nullptr);
    void processFile(const QString &fileName);

private:
    QHash<QString, QVector<HashFileInfoStruct> > hashByHash;
    //QCryptographicHash *hashAlgo;
    int resultCount;
    void clearNoDuplicatedSize();
    void makeHashByHashes();
    void clearNoDuplicatedHashes();
    QList<HashFileInfoStruct> * reduceToResult();

signals:
    void finished();
    void finishedWData(QList<HashFileInfoStruct> *items = nullptr);

public slots:
    void process();
};

#endif // DIRCOMPARATOR_H
