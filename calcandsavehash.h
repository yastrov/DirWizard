#ifndef CALCANDSAVEHASH_H
#define CALCANDSAVEHASH_H
#include <QDebug>
#include <QCryptographicHash>
#include "hashdirwalker.h"
#include "hashfileinfostruct.h"

class CalcAndSaveHash : public HashDirWalker
{
    Q_OBJECT
public:
    CalcAndSaveHash(QCryptographicHash::Algorithm hash, QObject *parent = nullptr);
    QString getHashFileExtention(QCryptographicHash::Algorithm algoEnum);

protected:
    void processFile(const QString &fileName);

private:
    QString hashFileExtention;
    void saveHashToFile(const QString &fileName, const QString &hashData);

signals:
    void finishedWData(QList<HashFileInfoStruct> *items = nullptr);

public slots:
    void process();
};

#endif // CALCANDSAVEHASH_H
