#ifndef HASHDIRWALKER_H
#define HASHDIRWALKER_H

#include <QObject>
#include <QFile>
#include <QCryptographicHash>
#include <QDir>
#include <QList>
#include <QVector>
#include <QDirIterator>
#include <QDebug>

class HashDirWalker : public QObject
{
    Q_OBJECT
public:
    explicit HashDirWalker(QCryptographicHash::Algorithm hash, QObject *parent = 0);
    static QByteArray fileChecksum(const QString &fileName, QCryptographicHash &hash);
    static QByteArray fileChecksum(const QString &fileName, QCryptographicHash *hash);
    static QByteArray fileChecksum(const QString &fileName);
    virtual void processFile(const QString &fileName) = 0;

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
    QCryptographicHash *hashAlgo;
    QList<QDir> rootDirs;

signals:
    void finished();

public slots:
    void stop();

private:

};

#endif // HASHDIRWALKER_H
