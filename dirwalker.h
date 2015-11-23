#ifndef DIRWALKER_H
#define DIRWALKER_H

#include <QObject>
#include <QDir>
#include <QList>
#include <QVector>
#include <QDirIterator>
#include <QThread>

class DirWalker : public QObject
{
    Q_OBJECT
public:
    explicit DirWalker(QObject *parent = 0);
    virtual ~DirWalker();
    void setQDir(const QDir &rootDir);
    void setQDir(const QList<QDir> &rootDirs);
    void setQDir(const QVector<QDir> &rootDirs);
    void setQDir(const QString &rootDir);
    void setQDir(const QList<QString> &rootDirs);
    void setQDir(const QVector<QString> &rootDirs);

protected:
    bool stopped;
    void processFilesRecursively(const QDir &rootDir);
    void processFilesRecursively(const QList<QDir> &rootDirs);
    void processFilesRecursively(const QVector<QDir> &rootDirs);
    QList<QDir> rootDirs;
    virtual void processFile(const QString &fileName) = 0;

signals:
    void finished();

public slots:
};

#endif // DIRWALKER_H
