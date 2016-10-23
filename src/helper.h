#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QDir>

namespace HELPER {

QString getTranslateLocation() {
#ifdef Q_OS_X11
    QString localizeFile = PREFIX;
    localizeFile.append("/share/qtagger/localize/qtagger_"+Config::getSysLang()+".qm");
    return localizeFile;
#endif
#ifdef Q_OS_WIN // QLocale::system().name()
    const QString nativeSeparator = QDir::toNativeSeparators(QDir::separator());
    return QDir::toNativeSeparators(QDir::currentPath())+nativeSeparator+ "translations" +nativeSeparator+ "dirwizard_" + QLocale::system().name();
#endif
}

}
#endif // HELPER_H
