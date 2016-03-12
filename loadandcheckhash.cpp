#include "loadandcheckhash.h"

LoadAndCheckHash::LoadAndCheckHash(QCryptographicHash::Algorithm hash, QObject *parent):
    HashDirWalker(hash, parent),
    algo(hash),
    itemsList(new QList<HashFileInfoStruct>())
{

}
LoadAndCheckHash::~ LoadAndCheckHash()
{

}

void LoadAndCheckHash::process()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "LoadAndCheckHash::process";
#endif
    QMutableListIterator<QDir> it(rootDirs);
    QDir current;
    QStringList qsl = getFilterExtention(algo);
    while(it.hasNext())
    {
        current = it.next();
        current.setNameFilters(qsl);
        it.setValue(current);
    }
    processFilesRecursively(rootDirs);
    emit finishedWData(itemsList);
    emit finished();
}

void LoadAndCheckHash::processFile(const QString &fileName)
{
    if(fileName.isEmpty() || fileName.isNull())
        return;
#ifdef MYPREFIX_DEBUG
    qDebug() << "LoadAndCheckHash::processFile";
#endif
    QFileInfo fInfo(fileName);
    QString hashStrFromFile,
            origFileName;
    if( fInfo.isFile() && fInfo.isReadable() )
    {

        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QIODevice::Text)) {
            while (!file.atEnd())
            {
                QByteArray bline = file.readLine();
                QString line = QString::fromUtf8(bline);

                QStringList qsl = line.split(" *");
                if(qsl.size() == 2)
                {
                    hashStrFromFile = qsl[0];
                    origFileName = qsl[1];
                    origFileName = QDir::toNativeSeparators(fInfo.path()) + QDir::separator() + origFileName;
                }
                else
                {
                    hashStrFromFile = line;
                    origFileName = QDir::toNativeSeparators(fInfo.path()) + QDir::separator() + fInfo.completeBaseName();
                }
            }
            file.close();
        }


#ifdef MYPREFIX_DEBUG
        qDebug() << "LoadAndCheckHash::processFile " << origFileName;
#endif
        if(QFileInfo(origFileName).exists())
        {
#ifdef MYPREFIX_DEBUG
        qDebug() << "LoadAndCheckHash::processFile File exists: " << origFileName;
#endif
            checkHash(origFileName, hashStrFromFile);
        }
        else
        {
#ifdef MYPREFIX_DEBUG
        qDebug() << "LoadAndCheckHash::processFile File non exists: " << origFileName;
#endif
            origFileName = QDir::toNativeSeparators(fInfo.path()) + QDir::separator() + fInfo.completeBaseName();
            if(QFileInfo(origFileName).exists())
            {
#ifdef MYPREFIX_DEBUG
        qDebug() << "LoadAndCheckHash::processFile File Exists: " << origFileName;
#endif
                checkHash(origFileName, hashStrFromFile);
            }
            else
            {
#ifdef MYPREFIX_DEBUG
        qDebug() << "LoadAndCheckHash::processFile File non exists: " << origFileName;
#endif
                HashFileInfoStruct strct;
                strct.hash = hashStrFromFile;
                strct.fileName = fileName;
                strct.checked = true;
                strct.size = 0;
                itemsList.data()->append(std::move(strct));
            }
        }
    }
}

QStringList LoadAndCheckHash::getFilterExtention(const QCryptographicHash::Algorithm &algoEnum)
{
    QStringList filters;
    switch(algoEnum)
    {
        case QCryptographicHash::Md5: filters << "*.md5"; break;
        case QCryptographicHash::Sha1: filters << "*.sha"; break;
        case QCryptographicHash::Sha256: filters << "*.sha256"; break;
        case QCryptographicHash::Sha512: filters << "*.sha512"; break;
        case QCryptographicHash::Sha3_256: filters << "*.sha3256"; break;
        case QCryptographicHash::Sha3_512: filters << "*.sha3512"; break;
    default:
        filters << "*.*";
        break;
    }
    return filters;
}

bool LoadAndCheckHash::checkHash(const QString &fileName,
                                const QString &hashFromFile)
{
    QByteArray qb = fileChecksum(fileName, hashAlgo);
    if(!qb.isNull() && !qb.isEmpty())
    {
        QString hashFromDisk = QString(qb);
        if(0!= hashFromDisk.compare(hashFromFile, Qt::CaseInsensitive))
        {
            HashFileInfoStruct strct;
            strct.hash = hashFromFile;
            strct.fileName = fileName;
            strct.checked = true;
            strct.size = QFileInfo(fileName).size();
            itemsList.data()->append(strct);
        }
        return true;
    }
    else return false;
}
