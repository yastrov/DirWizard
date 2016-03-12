#include "mainwindow.h"
#include <QApplication>
#include <QFileInfo>
#include <QCoreApplication>
#include "hashfileinfostruct.h"
#include <QSharedPointer>

int main(int argc, char *argv[])
{
    qRegisterMetaType<QSharedPointer<QList<HashFileInfoStruct>>>("QSharedPointer<QList<HashFileInfoStruct>>");
    qRegisterMetaType<QSharedPtrListHFIS>("QSharedPtrListHFIS");
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("platforms");
    QCoreApplication::setLibraryPaths(paths);

    QApplication a(argc, argv);
    a.setApplicationVersion(APP_VERSION);
    a.setApplicationName("DirWizard");

    MainWindow w;
    w.show();

    return a.exec();
}
