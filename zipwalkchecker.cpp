#include "zipwalkchecker.h"

ZipWalkChecker::ZipWalkChecker(QObject *parent):
    DirWalker(parent),
    itemsList(new QList<HashFileInfoStruct>())
{

}

ZipWalkChecker::~ZipWalkChecker()
{

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
    calcTotalFiles();
    emit sayTotalFiles(total_files);
    DirWalker::processFilesRecursively(rootDirs);
    emit currentProcessedFiles(processed_files);
    emit finishedWData(itemsList);
    emit finished();
}

void ZipWalkChecker::processFile(const QString &fileName)
{
    if(fileName.isEmpty() || fileName.isNull())
        return;
#ifdef MYPREFIX_DEBUG
    qDebug() << "ZipWalkChecker::processFile: " << fileName;
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
            fs.checked = true;
            itemsList->append(fs);
        }
        if(qzip.isOpen())
            qzip.close();
    }
    ++processed_files;
    if(processed_files %10 == 0)
        emit currentProcessedFiles(processed_files);
}

