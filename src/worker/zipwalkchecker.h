#ifndef ZIPWALKCHECKER_H
#define ZIPWALKCHECKER_H
#include <QObject>
#include "dirwalker.h"
#include "../hashfileinfostruct.h"

#ifdef Q_OS_WIN
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include <quazip/quazipfileinfo.h>
#include <quazip/quacrc32.h>
#else
#include <quazip.h>
#include <quazipfile.h>
#include <quazipfileinfo.h>
#include <quacrc32.h>
#endif

#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif
#include "../constants.h"
#include <QLinkedList>

class ZipWalkChecker : public DirWalker
{
    Q_OBJECT
public:
    explicit ZipWalkChecker(QObject *parent = nullptr);
    virtual ~ZipWalkChecker();

protected:

private:
    QLinkedList<HashFileInfoStruct> tempList;
    void processFile(const QString &fileName) Q_DECL_OVERRIDE;
    QSharedPtrListHFIS result;
    const static qint64 bufferSize=4096;
    char buffer[bufferSize];

signals:
    void finishedWData(QSharedPtrListHFIS itemsPtr);

public slots:
    void process();
};

#endif // ZIPWALKCHECKER_H
