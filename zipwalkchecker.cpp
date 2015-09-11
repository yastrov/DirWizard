#include "zipwalkchecker.h"

ZipWalkChecker::ZipWalkChecker(QObject *parent):
    DirWalker(parent),
    itemsList(new QList<HashFileInfoStruct>())
{

}

void ZipWalkChecker::stop()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "ZipWalkChecker::stop";
#endif
    stopped = true;
}

void ZipWalkChecker::process()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "ZipWalkChecker::process";
#endif
    QStringList filters;
    QDir current;
    QMutableListIterator<QDir> it(rootDirs);
    filters << "*.zip";
    while(it.hasNext())
    {
        current = it.next();
        current.setNameFilters(filters);
        it.setValue(current);
    }
    processFilesRecursively(rootDirs);
    emit finishedWData(itemsList);
    emit finished();
}

void ZipWalkChecker::processFile(const QString &fileName)
{
    if(fileName.isEmpty() || fileName.isNull())
        return;
#ifdef MYPREFIX_DEBUG
    qDebug() << "ZipWalkChecker::processFile";
#endif
    QFileInfo fInfo(fileName);
    if( fInfo.isFile() && fInfo.isReadable() )
    {
        QuaZip qzip(fileName);
        if(qzip.open(QuaZip::mdUnzip))
        {
            qzip.goToFirstFile();
        }
        if(qzip.getZipError() != UNZ_OK)
        {
            HashFileInfoStruct fs;
            fs.fileName = fileName;
            fs.size = fInfo.size();
            itemsList->append(fs);
        }
        if(qzip.isOpen())
            qzip.close();
    }
}
