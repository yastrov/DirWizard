#include "zipwalkchecker.h"

ZipWalkChecker::ZipWalkChecker(QObject *parent):
    DirWalker(parent)
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
    DirWalker::processFilesRecursively();

    result = QSharedPtrListHFIS(new QList<HashFileInfoStruct>());
    QList<HashFileInfoStruct> * const list = result.data();
    list->reserve(processed_files);

    while (!tempList.isEmpty()) {
        list->append(std::move(tempList.takeFirst()));
    }

    emit currentProcessedFiles(processed_files);
    emit finishedWData(result);
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
        bool isDamagedZip = false;
        if(qzip.open(QuaZip::mdUnzip))
        {
            QuaZipFile file(&qzip);
            QuaZipFileInfo64 zipInfo;
            QuaCrc32 crc32Obj;

            for(bool more=qzip.goToFirstFile(); more; more=qzip.goToNextFile()) {
                crc32Obj.reset();
                if (!qzip.getCurrentFileInfo(&zipInfo)) {
                    isDamagedZip = true;
                    break;
                }
                if(file.open(QIODevice::ReadOnly)) {
#ifdef MYPREFIX_DEBUG
                    qDebug() << "innerFile: " << file.getActualFileName();
#endif
                    while(!file.atEnd()) {
                        const qint64 readLen = file.read(buffer, bufferSize);
                        if (readLen <= 0)
                            break;
                        crc32Obj.update(QByteArray(buffer, readLen));
                    }
                    file.close();
                    if(crc32Obj.value() != zipInfo.crc) {
                        isDamagedZip = true;
                        break;
                    }

                } else {
                    if(qzip.getZipError() != UNZ_OK) {
                        isDamagedZip = true;
                        break;
                    }
                }
            }
        }
        if(qzip.isOpen())
            qzip.close();
        if(qzip.getZipError() != UNZ_OK) {
            isDamagedZip = true;
        }

        if(isDamagedZip) {
            HashFileInfoStruct fs;
            fs.fileName = fileName;
            fs.checked = true;
            tempList.append(fs);
        }
        ++processed_files;
        if(processed_files % SAY_PROGRESS_EVERY == 0)
            emit currentProcessedFiles(processed_files);
    }
}

