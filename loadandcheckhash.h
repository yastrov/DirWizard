#ifndef LOADANDCHECKHASH_H
#define LOADANDCHECKHASH_H
#include "hashdirwalker.h"
#include "hashfileinfostruct.h"
#include <QObject>
#include <QCryptographicHash>
#include <QDir>
#include <QThread>

class LoadAndCheckHash : public HashDirWalker
{
    Q_OBJECT
public:
    LoadAndCheckHash(QCryptographicHash::Algorithm hash, QObject *parent = nullptr);
    virtual ~ LoadAndCheckHash();
    QStringList getFilterExtention(const QCryptographicHash::Algorithm &algoEnum);

protected:
    void processFile(const QString &fileName) Q_DECL_OVERRIDE;

private:
    QCryptographicHash::Algorithm algo;
    QList<HashFileInfoStruct> *itemsList;
    QString hashFileExtention;
    void LoadHashFromFile(const QString &fileName, const QString &hashData);
    bool checkHash(const QString &fileName, const QString &hashFromFile);

signals:
    void finishedWData(QList<HashFileInfoStruct> *items = nullptr);

public slots:
    void process();
};

#endif // LOADANDCHECKHASH_H
