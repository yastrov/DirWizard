#ifndef DUPLICATEFINDER_H
#define DUPLICATEFINDER_H
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

class DuplicateFinder : public HashDirWalker
{
    Q_OBJECT
public:
    DuplicateFinder(QCryptographicHash::Algorithm hash, QObject *parent = nullptr);
    void processFile(const QString &fileName);

private:
    QHash<qint64, QVector<HashFileInfoStruct> > hashBySize;
    QHash<QString, QVector<HashFileInfoStruct> > hashByHash;
    //QCryptographicHash *hashAlgo;
    int resultCount;
    void clearNoDuplicatedSize();
    void makeHashByHashes();
    void clearNoDuplicatedHashes();
    QList<HashFileInfoStruct> * reduceToResult();

signals:
    void finishedWData(QList<HashFileInfoStruct> *items = nullptr);

public slots:
    void process();

};

#endif // DUPLICATEFINDER_H
