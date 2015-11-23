#include "mainwindow.h"
#include <QApplication>
#include <QFileInfo>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
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
