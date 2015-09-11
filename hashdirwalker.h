#ifndef HASHDIRWALKER_H
#define HASHDIRWALKER_H

#include <QObject>
#include <QFile>
#include <QCryptographicHash>
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif
#include "dirwalker.h"

class HashDirWalker : public DirWalker
{
    Q_OBJECT
public:
    explicit HashDirWalker(QCryptographicHash::Algorithm hash, QObject *parent = nullptr);
    static QByteArray fileChecksum(const QString &fileName, QCryptographicHash &hash);
    static QByteArray fileChecksum(const QString &fileName, QCryptographicHash *hash);
    static QByteArray fileChecksum(const QString &fileName);

protected:
    QCryptographicHash *hashAlgo;

signals:


public slots:
    void stop();

private:

};

#endif // HASHDIRWALKER_H
