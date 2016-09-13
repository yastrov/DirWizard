#include "dirwalker.h"
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif

DirWalker::DirWalker(QObject *parent) : QObject(parent),
    stopped(false), total_files(0), processed_files(0)
{

}

DirWalker::~DirWalker()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DirWalker::~DirWalker";
#endif
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
        if(QThread::currentThread()->isInterruptionRequested())
        stopped=true;
    }
    if(stopped)
    {
        emit finished();
    }
}

void DirWalker::calcTotalFiles()
{
    QListIterator<QDir> it0(rootDirs);
    while(it0.hasNext()) {
        const QDir &dir = it0.next();
        QDirIterator it(dir, QDirIterator::Subdirectories);
        while(it.hasNext() && !stopped) {
            ++total_files;
            it.next();
            if(QThread::currentThread()->isInterruptionRequested())
            stopped=true;
        }
        if(stopped)
        {
            emit finished();
        }
    }
}

void DirWalker::setFilters(const QStringList &filters)
{
    QDir current;
    QMutableListIterator<QDir> it(rootDirs);
    while(it.hasNext())
    {
        current = it.next();
        current.setNameFilters(filters);
        it.setValue(current);
    }
}
