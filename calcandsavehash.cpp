#include "calcandsavehash.h"

CalcAndSaveHash::CalcAndSaveHash(QCryptographicHash::Algorithm hash, QObject *parent):
    HashDirWalker(hash, parent),
    hashFileExtention(getHashFileExtention(hash))
{

}

CalcAndSaveHash::~CalcAndSaveHash()
{

}

void CalcAndSaveHash::process()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "CalcAndSaveHash::process";
#endif
    calcTotalFiles();
    emit sayTotalFiles(total_files);
    DirWalker::processFilesRecursively(rootDirs);
    emit currentProcessedFiles(processed_files);
    emit finished();
}

void CalcAndSaveHash::processFile(const QString &fileName)
{
    if(fileName.isEmpty() || fileName.isNull())
        return;
#ifdef MYPREFIX_DEBUG
    qDebug() << "CalcAndSaveHash::processFile: " << fileName;
#endif
    QFileInfo fInfo(fileName);
    QString hashStr;
    if( fInfo.isFile() && fInfo.isReadable() )
    {
        QByteArray qb = fileChecksum(fileName, hashAlgo);
        if(!qb.isNull() && !qb.isEmpty())
        {
            saveHashToFile(fileName, QString(qb));
        }
        ++processed_files;
    }
}

QString CalcAndSaveHash::getHashFileExtention(QCryptographicHash::Algorithm algoEnum)
{
    switch(algoEnum)
    {
        case QCryptographicHash::Md5: return ".md5"; break;
        case QCryptographicHash::Sha1: return ".sha"; break;
        case QCryptographicHash::Sha256: return ".sha256"; break;
        case QCryptographicHash::Sha512: return ".sha512"; break;
        case QCryptographicHash::Sha3_256: return ".sha3256"; break;
        case QCryptographicHash::Sha3_512: return ".sha3512"; break;
        default: return ""; break;
    }
}

void CalcAndSaveHash::saveHashToFile(const QString &fileName, const QString &hashData)
{
    QString outputFileName(fileName);
    outputFileName = outputFileName.append(hashFileExtention);
    QFile file(outputFileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QFileInfo fi(fileName);
        QString name = fi.fileName();

        QTextStream out(&file);
        out.setCodec("UTF-8");
        out.setGenerateByteOrderMark(false);
        out << hashData << " *" << name;
        file.close();
    }
#ifdef MYPREFIX_DEBUG
    else {
        qDebug() << "CalcAndSaveHash::saveHashToFile Problem with saving hash for file: " << outputFileName;
    }
#endif
}

