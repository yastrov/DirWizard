#ifndef EMPTYDIRFINDER_H
#define EMPTYDIRFINDER_H

#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif
#include <QObject>
#include <QLinkedList>
#include "dirwalker.h"
#include "../hashfileinfostruct.h"
#include "../constants.h"

class EmptyDirFinder : public DirWalker
{
    Q_OBJECT
public:
    explicit EmptyDirFinder(QObject *parent = nullptr);
    virtual ~EmptyDirFinder(){}
    void processFile(const QString &fileName) Q_DECL_OVERRIDE;

private:
    QLinkedList<HashFileInfoStruct> tempList;
    QSharedPtrListHFIS result;

public slots:
    void process();

signals:
    void finishedWData(QSharedPtrListHFIS itemsPtr);
};

#endif // EMPTYDIRFINDER_H
