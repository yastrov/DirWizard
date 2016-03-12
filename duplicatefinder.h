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
#include <QThread>
#include <hashfileinfostruct.h>
#include <hashdirwalker.h>
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif

#define SizeForInnerVector 0

class DuplicateFinder : public HashDirWalker
{
    Q_OBJECT
public:
    DuplicateFinder(QCryptographicHash::Algorithm hash, QObject *parent = nullptr);
    virtual ~DuplicateFinder();
    void processFile(const QString &fileName) Q_DECL_OVERRIDE;

private:
    QHash<qint64, QVector<HashFileInfoStruct> > hashBySize;
    QHash<QString, QVector<HashFileInfoStruct> > hashByHash;
    int resultCount;
    void clearNoDuplicatedSize();
    void makeHashByHashes();
    void clearNoDuplicatedHashes();
    void reduceToResult();
    QSharedPtrListHFIS result;

signals:
    void finishedWData(QSharedPtrListHFIS itemsPtr);

public slots:
    void process();

};

#endif // DUPLICATEFINDER_H
