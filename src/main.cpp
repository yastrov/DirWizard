#include "src/gui/mainwindow.h"
#include <QApplication>
#include "hashfileinfostruct.h"
#include <QSharedPointer>
#include <QTranslator>
#include <QLibraryInfo>
#include "src/helper.h"
#include "constants.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<QSharedPointer<QList<HashFileInfoStruct>>>("QSharedPointer<QList<HashFileInfoStruct>>");
    qRegisterMetaType<QSharedPtrListHFIS>("QSharedPtrListHFIS");
    qRegisterMetaType<q_coll_s_t>("q_coll_s_t");

    QApplication a(argc, argv);
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load(HELPER::getTranslateLocation());
    a.installTranslator(&myappTranslator);

    a.setApplicationVersion(APP_VERSION);
    a.setApplicationName("DirWizard");

    MainWindow w;
    w.show();

    return a.exec();
}
