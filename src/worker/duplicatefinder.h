#ifndef DUPLICATEFINDER_H
#define DUPLICATEFINDER_H
#include <QObject>
#include <QMutableListIterator>
#include <QListIterator>
#include <QList>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QThread>
#include "../hashfileinfostruct.h"
#include "../worker/hashdirwalker.h"
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif
#include "../constants.h"

#define SizeForInnerVector 0

class DuplicateFinder : public HashDirWalker
{
    Q_OBJECT
public:
    DuplicateFinder(QCryptographicHash::Algorithm hash, QObject *parent = nullptr);
    virtual ~DuplicateFinder();
    void processFile(const QString &fileName) Q_DECL_OVERRIDE;

private:
    QMultiHash<qint64, HashFileInfoStruct> hashBySize;
    QMultiHash<QString, HashFileInfoStruct> hashByHash;
    q_coll_s_t resultCount;
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
