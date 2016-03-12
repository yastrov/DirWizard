#ifndef HASHFILEINFOSTRUCT
#define HASHFILEINFOSTRUCT
#include <QString>
#include <QSharedPointer>

typedef struct {
    //QFileInfo i;
    QString fileName;
    QString hash;
    bool checked;
    uint groupID;
    qint64 size;
} HashFileInfoStruct;

typedef QSharedPointer<QList<HashFileInfoStruct>> QSharedPtrListHFIS;

#endif // HASHFILEINFOSTRUCT
