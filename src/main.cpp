#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    a.setStyle("Fusion");

    MainWindow w;

    QObject::connect(&w, SIGNAL(finished()), &a, SLOT(quit()));
    a.setQuitOnLastWindowClosed(false);

    if (!a.arguments().contains("nogui") )
        w.show();

    if (a.arguments().contains("autostart"))
        w.startMonitoring();

    return a.exec();
}
