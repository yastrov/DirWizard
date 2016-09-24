#include "hashdirwalker.h"
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif

HashDirWalker::HashDirWalker(QCryptographicHash::Algorithm hash, QObject *parent):
    DirWalker(parent),
    hashAlgo(new QCryptographicHash(hash))
{

}
HashDirWalker::~HashDirWalker()
{
    if(hashAlgo) {
        delete hashAlgo;
    }
}

QByteArray HashDirWalker::fileChecksum(const QString &fileName,
                                    QCryptographicHash &hash)
{
    hash.reset();
#ifdef MYPREFIX_DEBUG
    qDebug() << fileName;
#endif
    QFile file(fileName);
        if (file.open(QFile::ReadOnly)) {
            while (!file.atEnd())
            {
                hash.addData(file.readLine());
            }
            file.close();
            return hash.result().toHex();
        }
    return QByteArray();
}

QByteArray HashDirWalker::fileChecksum(const QString &fileName,
                                    QCryptographicHash *hash)
{
    hash->reset();
#ifdef MYPREFIX_DEBUG
    qDebug() <<"HashDirWalker::fileChecksum "<< fileName;
#endif
    QFile file(fileName);
        if (file.open(QFile::ReadOnly)) {
            while (!file.atEnd())
            {
                hash->addData(file.readLine());
            }
            file.close();
            return hash->result().toHex();
        }
    return QByteArray();
}
