#include "hashdirwalker.h"

HashDirWalker::HashDirWalker(QCryptographicHash::Algorithm hash, QObject *parent):
    QObject(parent),
    stopped(false),
    hashAlgo(new QCryptographicHash(hash))
{

}

void HashDirWalker::stop()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::stop";
 #endif
    stopped = true;
}


void HashDirWalker::setQDir(const QDir &rootDir)
{
    rootDirs.append(rootDir);
}

void HashDirWalker::setQDir(const QList<QDir> &rootDirs)
{
    this->rootDirs = QList<QDir>(rootDirs);
}

void HashDirWalker::setQDir(const QVector<QDir> &rootDirs)
{
    this->rootDirs = QList<QDir>(rootDirs.toList());
}

void HashDirWalker::setQDir(const QString &rootDir)
{
    rootDirs.append(QDir(rootDir));
}

void HashDirWalker::setQDir(const QList<QString> &rootDirs)
{
    QListIterator<QString> it (rootDirs);
    while(it.hasNext())
        this->rootDirs.append(QDir(it.next()));
}

void HashDirWalker::setQDir(const QVector<QString> &rootDirs)
{
    QVectorIterator<QString> it (rootDirs);
    while(it.hasNext())
        this->rootDirs.append(QDir(it.next()));
}

void HashDirWalker::processFilesRecursively(const QList<QDir> &rootDirs)
{
    QListIterator<QDir> i(rootDirs);
    while (i.hasNext())
        processFilesRecursively(i.next());
}

void HashDirWalker::processFilesRecursively(const QVector<QDir> &rootDirs)
{
    QVectorIterator<QDir> i(rootDirs);
    while (i.hasNext())
    {
        processFilesRecursively(i.next());
    }
}

void HashDirWalker::processFilesRecursively(const QDir &rootDir) {
    QDirIterator it(rootDir, QDirIterator::Subdirectories);
    while(it.hasNext() && !stopped) {
        processFile(it.next());
    }
    if(stopped)
    {
        emit finished();
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
    qDebug() << fileName;
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
