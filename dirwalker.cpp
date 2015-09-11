#include "dirwalker.h"

DirWalker::DirWalker(QObject *parent) : QObject(parent),
    stopped(false)
{

}

void DirWalker::setQDir(const QDir &rootDir)
{
    rootDirs.append(rootDir);
}

void DirWalker::setQDir(const QList<QDir> &rootDirs)
{
    this->rootDirs = QList<QDir>(rootDirs);
}

void DirWalker::setQDir(const QVector<QDir> &rootDirs)
{
    this->rootDirs = QList<QDir>(rootDirs.toList());
}

void DirWalker::setQDir(const QString &rootDir)
{
    rootDirs.append(QDir(rootDir));
}

void DirWalker::setQDir(const QList<QString> &rootDirs)
{
    QListIterator<QString> it (rootDirs);
    while(it.hasNext())
        this->rootDirs.append(QDir(it.next()));
}

void DirWalker::setQDir(const QVector<QString> &rootDirs)
{
    QVectorIterator<QString> it (rootDirs);
    while(it.hasNext())
        this->rootDirs.append(QDir(it.next()));
}

void DirWalker::processFilesRecursively(const QList<QDir> &rootDirs)
{
    QListIterator<QDir> i(rootDirs);
    while (i.hasNext())
        processFilesRecursively(i.next());
}

void DirWalker::processFilesRecursively(const QVector<QDir> &rootDirs)
{
    QVectorIterator<QDir> i(rootDirs);
    while (i.hasNext())
    {
        processFilesRecursively(i.next());
    }
}

void DirWalker::processFilesRecursively(const QDir &rootDir) {
    QDirIterator it(rootDir, QDirIterator::Subdirectories);
    while(it.hasNext() && !stopped) {
        processFile(it.next());
    }
    if(stopped)
    {
        emit finished();
    }
}
