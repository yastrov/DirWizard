#ifndef DIRCOMPARATOR_H
#define DIRCOMPARATOR_H
#include <QObject>
#include <QMutableListIterator>
#include <QList>
#include <QFileInfo>
#include <QCryptographicHash>
#include "../hashfileinfostruct.h"
#include "hashdirwalker.h"
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif
#include <QThread>
#include "../constants.h"

#define SizeForInnerVector 0

class DirComparator : public HashDirWalker
{
     Q_OBJECT
public:
    DirComparator(QCryptographicHash::Algorithm hash, QObject *parent = nullptr);
    virtual ~DirComparator();
    void processFile(const QString &fileName) Q_DECL_OVERRIDE;

protected:

private:
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

#endif // DIRCOMPARATOR_H
