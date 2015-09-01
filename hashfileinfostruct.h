#ifndef HASHFILEINFOSTRUCT
#define HASHFILEINFOSTRUCT
#include <QString>

typedef struct {
    //QFileInfo i;
    QString fileName;
    QString hash;
    bool checked;
    uint groupID;
    qint64 size;
} HashFileInfoStruct;

#endif // HASHFILEINFOSTRUCT
