#ifndef CALCANDSAVEHASH_H
#define CALCANDSAVEHASH_H
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif
#include <QCryptographicHash>
#include "hashdirwalker.h"
#include "hashfileinfostruct.h"
#include <QThread>

class CalcAndSaveHash : public HashDirWalker
{
    Q_OBJECT
public:
    CalcAndSaveHash(QCryptographicHash::Algorithm hash, QObject *parent = nullptr);
    ~CalcAndSaveHash();
    QString getHashFileExtention(QCryptographicHash::Algorithm algoEnum);

protected:
    void processFile(const QString &fileName) Q_DECL_OVERRIDE;

private:
    QString hashFileExtention;
    void saveHashToFile(const QString &fileName, const QString &hashData);

signals:
    void finishedWData(QList<HashFileInfoStruct> *items = nullptr);

public slots:
    void process();
};

#endif // CALCANDSAVEHASH_H
