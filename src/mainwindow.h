#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "programtablemodel.h"
#include "dialogs/editprogramdialog.h"
#include "dialogs/nameanddescriptiondialog.h"

#include <QMainWindow>
#include <QSettings>
#include <QProcess>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QMenu>
#include <QStringList>
#include <QCloseEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QSystemTrayIcon>
#include <QFileIconProvider>

#include <windows.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadSettings();
    void saveSettings();

    void startMonitoring();

signals:
    void finished();

private slots:
    void on_cmdAddProgram_clicked();
    void on_cmdEditProgram_clicked();
    void on_cmdDeleteProgram_clicked();

    void on_actionStartMonitoring_triggered();
    void on_actionStopMonitoring_triggered();

    void onTimeout();
    void restartProgram(int index, bool kill = false);

    void onCloseRequest();

    void closeEvent(QCloseEvent *event);
    void hideEvent(QHideEvent *event);

    void onSystemTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void notifyBackground();

    void on_tblProgramList_customContextMenuRequested(const QPoint &pos);
    void on_tblProgramList_clicked(const QModelIndex &index);
    void on_tblProgramList_doubleClicked(const QModelIndex &index);

    void on_actionQuit_triggered();
    void on_cmdEditNameDescription_clicked();

    void displayInfo();

    void on_actionEnableNotifications_triggered(bool checked);

private:
    bool isProcessHanging(int pid);
    void logMessage(QString level, QString msg, QString fileName = "");

    Ui::MainWindow *ui;

    ProgramTableModel *m_model;
    QList<QTimer*> m_timerList;

    QSystemTrayIcon *m_sysTrayIcon;
    QAction *m_statusAction;
    QAction *m_startAction;
    QAction *m_stopAction;

    bool m_running;
    bool m_bgNotified;
    bool m_enableNotifications;

    QMenu *m_contextMenu;

    QFileIconProvider m_iconProvider;
};

#endif // MAINWINDOW_H
