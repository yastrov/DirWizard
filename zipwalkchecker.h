#ifndef ZIPWALKCHECKER_H
#define ZIPWALKCHECKER_H
#include <QObject>
#include "dirwalker.h"
#include "hashfileinfostruct.h"
#include <quazip/quazip.h>
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif
#include "constants.h"

class ZipWalkChecker : public DirWalker
{
    Q_OBJECT
public:
    explicit ZipWalkChecker(QObject *parent = nullptr);
    virtual ~ZipWalkChecker();

protected:

private:
    QSharedPtrListHFIS itemsList;
    void processFile(const QString &fileName) Q_DECL_OVERRIDE;
    QSharedPtrListHFIS result;

signals:
    void finishedWData(QSharedPtrListHFIS itemsPtr);

public slots:
    void process();
};

#endif // ZIPWALKCHECKER_H
