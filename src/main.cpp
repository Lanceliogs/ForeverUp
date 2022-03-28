#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QLockFile>
#include <QSharedMemory>
#include <QTranslator>

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    a.setStyle("Fusion");

    // Lock file guard
    QLockFile lockFile(QString("%1/foreverup.pid").arg(QDir::tempPath()));
    lockFile.setStaleLockTime(0);
    if (!lockFile.tryLock(100)) {
        qDebug() << "ForeverUp is already running";
        qint64 pid; QString hostName, appName;
        lockFile.getLockInfo(&pid, &hostName, &appName);

        QSharedMemory shared(FUP_SHARED_MEM_KEY);
        if (!shared.attach()) {
            qDebug() << "Can't attach to shared memory. Maybe the first instance doesn't run correctly.";
            return 1;
        }
        if (!shared.lock()) {
            qDebug() << "Couldn't lock the shared memory, nothing will be done.";
            return 1;
        }
        qint64 *showRequest = (qint64*)shared.data();
        *showRequest = pid;
        shared.unlock();
        shared.detach();
        qDebug() << "Show request sent to pid:" << pid;
        return 0;
    }

    // Translator
    QTranslator translator;
    qDebug() << "Loading translation:" << QLocale::countryToString(QLocale().country());
    if (translator.load(QLocale(), "foreverup", "_", a.applicationDirPath(), ".qm")) {
        qDebug() << "Found!";
        a.installTranslator(&translator);
    } else {
        qDebug() << "Translation not found, using english default language";
    }

    MainWindow w;

    QObject::connect(&w, SIGNAL(finished()), &a, SLOT(quit()));
    a.setQuitOnLastWindowClosed(false);

    if (!a.arguments().contains("nogui") )
        w.show();

    if (a.arguments().contains("autostart"))
        w.startMonitoring();

    return a.exec();
}
