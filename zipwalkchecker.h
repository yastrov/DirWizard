#ifndef ZIPWALKCHECKER_H
#define ZIPWALKCHECKER_H
#include <QObject>
#include "dirwalker.h"
#include "hashfileinfostruct.h"
#include <quazip/quazip.h>
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif

class ZipWalkChecker : public DirWalker
{
    Q_OBJECT
public:
    explicit ZipWalkChecker(QObject *parent = nullptr);

private:
    QList<HashFileInfoStruct> *itemsList;
    void processFile(const QString &fileName);

signals:
    void finishedWData(QList<HashFileInfoStruct> *items = nullptr);

public slots:
    void process();
};

#endif // ZIPWALKCHECKER_H
