#include "mainwindow.h"
#include "ui_mainwindow.h"

#define LL_INFO "INFO"
#define LL_WARNING "WARNING"
#define LL_FATAL "FATAL"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_running(false),
    m_bgNotified(false)
{
    ui->setupUi(this);

    m_about = new AboutWidget(this);

    m_settings = new AdvancedSettingsDialog(this);

    m_model = new ProgramTableModel(ui->tblProgramList);
    ui->tblProgramList->setModel(m_model);

    // System tray icon
    m_sysTrayIcon = new QSystemTrayIcon(this);
    m_sysTrayIcon->setIcon(QIcon(":/icon/FUP_icon_white.png"));
    connect(m_sysTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(onSystemTrayIconActivated(QSystemTrayIcon::ActivationReason)));
    m_sysTrayIcon->setToolTip(tr("ForeverUp - Stopped"));

    // Menu
    QMenu *menu = new QMenu(this);
    m_statusAction = menu->addAction(tr("ForeverUp - Stopped"));
    QAction *showAction = menu->addAction(tr("Open ForeverUp..."), this, SLOT(show()));
    QFont font = showAction->font();
    font.setBold(true);
    showAction->setFont(font);
    menu->addSeparator();
    m_startAction = menu->addAction(tr("Start monitoring"), this, SLOT(on_actionStartMonitoring_triggered()));
    m_stopAction = menu->addAction(tr("Stop monitoring"), this, SLOT(on_actionStopMonitoring_triggered()));
    menu->addSeparator();
    menu->addAction(tr("Exit"), this, SLOT(onCloseRequest()));
    m_sysTrayIcon->setContextMenu(menu);

    m_statusAction->setEnabled(false);
    ui->actionStartMonitoring->setEnabled(true);
    m_startAction->setEnabled(true);
    ui->actionStopMonitoring->setEnabled(false);
    m_stopAction->setEnabled(false);
    m_sysTrayIcon->setVisible(true);

    m_contextMenu = new QMenu(this);
    m_contextMenu->addAction(tr("Edit name/description"), this, SLOT(on_cmdEditNameDescription_clicked()));
    m_contextMenu->addAction(tr("Edit program settings"), this, SLOT(on_cmdEditProgram_clicked()));
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(tr("Delete"), this, SLOT(on_cmdDeleteProgram_clicked()));

    ui->txtLog->setMaximumBlockCount(1000);

    ui->actionEnableNotifications->setEnabled(m_sysTrayIcon->supportsMessages());
    m_enableNotifications = !qApp->arguments().contains("quiet") && m_sysTrayIcon->supportsMessages();
    ui->actionEnableNotifications->setChecked(m_enableNotifications);

    // Show request shared memory
    m_showRequest = new QSharedMemory(FUP_SHARED_MEM_KEY, this);
    if (m_showRequest->create(sizeof(qint64))) {
        m_checkShowReqTimer = new QTimer(this);
        connect(m_checkShowReqTimer, SIGNAL(timeout()), this, SLOT(onCheckShowRequestTimeout()));
        m_checkShowReqTimer->start(100);
    }
    else {
        qDebug() << "Couldn't create shared memory. Interprocess show requests won't be used.";
    }

    loadSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void MainWindow::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    notifyBackground();
}

void MainWindow::loadSettings()
{
    QString setFileName = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/foreverup.ini";
    QSettings set(setFileName, QSettings::IniFormat);
    set.setIniCodec("UTF-8");

    QStringList programNameList = set.childGroups();
    foreach(QString programName, programNameList) {
        set.beginGroup(programName);
        m_model->insertRow(m_model->rowCount());
        m_model->data().last().name = programName;
        m_model->data().last().index = m_model->rowCount();
        m_model->data().last().enabled = set.value("enabled", false).toBool();
        m_model->data().last().status = ProgramItem::Stopped;
        m_model->data().last().pid = 0;
        m_model->data().last().hangingCount = 0;
        m_model->data().last().restartCount = 0;
        m_model->data().last().lastRestartDateTime = QDateTime();
        m_model->data().last().restartError = false;
        m_model->data().last().description = set.value("description", "").toString();
        m_model->data().last().program = set.value("program", "").toString();
        m_model->data().last().arguments = set.value("arguments", "").toString();
        m_model->data().last().startIn = set.value("startIn", "").toString();
        m_model->data().last().checkInterval = set.value("checkInterval", "").toInt();
        m_model->data().last().memoryLimit = set.value("memoryLimit", 100).toInt();
        m_model->data().last().scriptBeforeRestart = set.value("scriptBeforeRestart", "").toString();

        QTimer *timer = new QTimer(this);
        timer->setInterval(m_model->data().last().checkInterval * 1000);
        m_timerList.append(timer);
        connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

        set.endGroup(); // programName
    }
}

void MainWindow::saveSettings()
{
    QString setFileName = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/foreverup.ini";
    QSettings set(setFileName, QSettings::IniFormat);
    set.setIniCodec("UTF-8");
    set.clear();
    set.sync();

    for (int i=0 ; i<m_model->rowCount() ; i++) {
        ProgramItem item = m_model->data()[i];
        set.beginGroup(item.name);
        set.setValue("enabled", item.enabled);
        set.setValue("description", item.description);
        set.setValue("program", item.program);
        set.setValue("arguments", item.arguments);
        set.setValue("startIn", item.startIn);
        set.setValue("checkInterval", item.checkInterval);
        set.setValue("memoryLimit", item.memoryLimit);
        set.setValue("logFile", item.logFile);
        set.setValue("scriptBeforeRestart", item.scriptBeforeRestart);
        set.endGroup(); // programName
    }
}

void MainWindow::on_cmdAddProgram_clicked()
{
    NameAndDescriptionDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    m_model->insertRow(m_model->rowCount());
    m_model->data().last().name = dialog.name();
    m_model->data().last().description = dialog.description();
    m_model->data().last().enabled = false;
    m_model->data().last().status = ProgramItem::Stopped;
    m_model->data().last().restartCount = 0;
    m_model->data().last().lastRestartDateTime = QDateTime();
    m_model->data().last().restartError = false;

    EditProgramDialog editDialog(this);
    editDialog.setWindowTitle(tr("Edit - %1").arg(dialog.name()));
    if (editDialog.exec() != QDialog::Accepted)
        return;

    m_model->data().last().program = editDialog.program();
    m_model->data().last().arguments = editDialog.arguments();
    m_model->data().last().startIn = editDialog.startIn();
    m_model->data().last().checkInterval = editDialog.checkInterval();
    m_model->data().last().memoryLimit = editDialog.memoryLimit();
    m_model->data().last().logFile = editDialog.logFile();
    m_model->data().last().scriptBeforeRestart = editDialog.scriptBeforeRestart();
    m_model->data().last().enabled = editDialog.isItemEnabled();

    m_timerList.append(new QTimer(this));
    m_timerList.last()->setInterval(m_model->data().last().checkInterval * 1000);
    connect(m_timerList.last(), SIGNAL(timeout()), this, SLOT(onTimeout()));
    if (m_running) m_timerList.last()->start();
}

void MainWindow::on_cmdEditProgram_clicked()
{
    QItemSelectionModel *select = ui->tblProgramList->selectionModel();
    if (!select->hasSelection())
        return;
    int row = select->selectedRows().first().row();
    ProgramItem item = m_model->data()[row];

    EditProgramDialog editDialog(this);
    editDialog.setProgram(item.program);
    editDialog.setArguments(item.arguments);
    editDialog.setStartIn(item.startIn);
    editDialog.setCheckInterval(item.checkInterval);
    editDialog.setMemoryLimit(item.memoryLimit);
    editDialog.setLogFile(item.logFile);
    editDialog.setScriptBeforeRestart(item.scriptBeforeRestart);
    editDialog.setIsItemEnabled(item.enabled);

    if (editDialog.exec() != QDialog::Accepted)
        return;

    m_model->data()[row].program = editDialog.program();
    m_model->data()[row].arguments = editDialog.arguments();
    m_model->data()[row].startIn = editDialog.startIn();
    m_model->data()[row].checkInterval = editDialog.checkInterval();
    m_model->data()[row].memoryLimit = editDialog.memoryLimit();
    m_model->data()[row].logFile = editDialog.logFile();
    m_model->data()[row].scriptBeforeRestart = editDialog.scriptBeforeRestart();
    m_model->data()[row].enabled = editDialog.isItemEnabled();

    bool active = m_timerList.at(row)->isActive();
    if (active) m_timerList.at(row)->stop();
    m_timerList.at(row)->setInterval(m_model->data()[row].checkInterval * 1000);
    if (active) m_timerList.at(row)->start();
}

void MainWindow::on_cmdDeleteProgram_clicked()
{
    QItemSelectionModel *select = ui->tblProgramList->selectionModel();
    if (!select->hasSelection())
        return;
    int row = select->selectedRows().first().row();
    m_model->removeRow(row);
    delete m_timerList.takeAt(row);
}

void MainWindow::on_actionStartMonitoring_triggered()
{
    startMonitoring();
}

void MainWindow::startMonitoring()
{
    if (m_running)
        return;
    m_running = true;
    for (int i=0 ; i<m_model->rowCount() ; i++) {
        if (!m_model->data()[i].enabled)
            continue;
        m_model->data()[i].status = ProgramItem::Running;
        m_model->data()[i].restartCount = 0;
        m_model->data()[i].hangingCount = 0;
        m_model->data()[i].restartError = false;
        m_timerList.at(i)->start();
    }

    if (m_enableNotifications)
        m_sysTrayIcon->showMessage(tr("ForeverUp"), tr("Monitoring started"), QIcon(":/icon/FUP_icon_white.png"), 1000);

    ui->actionMonitoringStatus->setIconText(tr("Running"));
    ui->actionMonitoringStatus->setIcon(QIcon(":/icon/witness_green.png"));

    ui->txtServiceStatus->setText("Running");
    m_statusAction->setText(tr("ForeverUp - Monitoring..."));
    m_sysTrayIcon->setToolTip(tr("ForeverUp - Monitoring..."));

    ui->actionStartMonitoring->setEnabled(false);
    m_startAction->setEnabled(false);
    ui->actionStopMonitoring->setEnabled(true);
    m_stopAction->setEnabled(true);

    logMessage(LL_INFO, "Monitoring started");
}

void MainWindow::on_actionStopMonitoring_triggered()
{
    if (!m_running)
        return;
    m_running = false;
    for (int i=0 ; i<m_model->rowCount() ; i++) {
        m_model->data()[i].pid = 0;
        m_model->data()[i].status = ProgramItem::Stopped;
        m_timerList.at(i)->stop();
    }

    if (m_enableNotifications)
        m_sysTrayIcon->showMessage(tr("ForeverUp"), tr("Monitoring stopped"), QIcon(":/icon/FUP_icon_white.png"), 1000);

    ui->actionMonitoringStatus->setIconText(tr("Stopped"));
    ui->actionMonitoringStatus->setIcon(QIcon(":/icon/invisible_red.png"));

    ui->txtServiceStatus->setText("Not running");
    m_statusAction->setText(tr("ForeverUp - Stopped"));
    m_sysTrayIcon->setToolTip(tr("ForeverUp - Stopped"));

    ui->actionStartMonitoring->setEnabled(true);
    m_startAction->setEnabled(true);
    ui->actionStopMonitoring->setEnabled(false);
    m_stopAction->setEnabled(false);

    logMessage(LL_INFO, "Monitoring stopped");
}

void MainWindow::onTimeout()
{
    int index = m_timerList.indexOf(qobject_cast<QTimer*>(sender()));
    ProgramItem item = m_model->data()[index];

    logMessage(LL_INFO, QString("Checking %1/%2").arg(item.name).arg(item.pid));

    // If pid is null, we need to check by name and determine the pid
    QString imageName = item.program.mid(item.program.lastIndexOf("/") + 1);
    QString command;
    if (item.pid == 0) {
        command = QString("tasklist /NH /FO csv /FI \"ImageName eq %1\" /FI \"Status eq Running\"").arg(imageName);
    } else {
        command = QString("tasklist /NH /FO csv /FI \"ImageName eq %1\" /FI \"PID eq %2\" /FI \"Status eq Running\"").arg(imageName).arg(item.pid);
    }
    logMessage(LL_INFO, QString("CMD: %1").arg(command));

    QProcess proc;
    proc.start(command);
    proc.waitForFinished(10000);
    QString cout = proc.readAllStandardOutput();
    logMessage(LL_INFO, QString("RET: %1").arg(cout));


    while (!cout.contains(imageName)) { // Not running
        if (item.pid == 0) {
            logMessage(LL_WARNING, QString("Not running: %1/%2").arg(item.name).arg(item.pid), item.logFile);
            restartProgram(index);
            return;
        } else {
            // PID may have changed, thus we reset it
            logMessage(LL_WARNING, QString("Instance was closed: %1/%2. Checking for another instance").arg(item.name).arg(item.pid), item.logFile);
            item.pid = 0;
            command = QString("tasklist /NH /FO csv /FI \"ImageName eq %1\" /FI \"Status eq Running\"").arg(imageName);
            logMessage(LL_INFO, QString("CMD: %1").arg(command));

            QProcess proc2;
            proc2.start(command);
            proc2.waitForFinished(10000);
            cout = proc2.readAllStandardOutput();
            logMessage(LL_INFO, QString("RET: %1").arg(cout));
        }
    }

    QStringList entries = cout.split("\r\n", QString::SkipEmptyParts);
    if (entries.size() > 1) {
        logMessage(LL_INFO, "More than one result, handling first entry only");
        cout = entries.first();
    }

    // Comma separated values and surrounded by ""
    QStringList tokens = cout.split("\",\"");
    cout.remove("\"");

    if (tokens.size() != 5) {
        logMessage(LL_WARNING, QString("Bad command return: %1").arg(cout), item.logFile);
        restartProgram(index);
        return;
    }

    // Retrieve PID if needed
    int pid = tokens.at(1).toInt();
    if (pid != item.pid) {
        m_model->data()[index].pid = pid;
        logMessage(LL_INFO, QString("New PID: %1").arg(m_model->data()[index].pid), item.logFile);
    }


    if (isProcessHanging(m_model->data()[index].pid)) {
        logMessage(LL_WARNING, QString("Process is hanging: %1").arg(item.pid), item.logFile);
        m_model->data()[index].maxHangingCount++;
        if (m_model->data()[index].maxHangingCount >= m_model->data()[index].maxHangingCount) {
            restartProgram(index, true); // Kill it first
            return;
        }
    }

    int memory = tokens.at(3).split(' ').first().remove(",").toInt() / 1024.;
    if (memory > item.memoryLimit) {
        logMessage(LL_WARNING, QString("Memory limit reached: %1 Mo > %2 Mo").arg(memory).arg(item.memoryLimit), item.logFile);
        restartProgram(index, true); // Kill it first
    }
}

void MainWindow::restartProgram(int index, bool kill)
{
    ProgramItem item = m_model->data()[index];
    QString imageName = item.program.mid(item.program.lastIndexOf("/") + 1);

    if (kill) {
        QString command = QString("taskkill %1 /T /F").arg(item.pid ? QString("/PID %1").arg(item.pid) : QString("/IM %1").arg(imageName));
        logMessage(LL_INFO, QString("CMD: %1").arg(command));
        if (QProcess::execute(command) < 0) {
            logMessage(LL_FATAL, QString("Couldn't kill the program: %1/%2").arg(item.name).arg(item.pid), item.logFile);
            m_model->data()[index].restartError = true;
            m_model->data()[index].status = ProgramItem::Stopped;
            displayInfo();
            return;
        }
    }

    // Run the before-restart script
    if (!item.scriptBeforeRestart.isEmpty()) {
        logMessage(LL_INFO, QString("CMD: %1").arg(item.scriptBeforeRestart));
        QProcess proc;
        proc.setWorkingDirectory(item.startIn);
        proc.start(item.scriptBeforeRestart);
        if (proc.waitForFinished(30000) && proc.exitCode() == 0) {
            logMessage(LL_INFO, QString("Before-restart command executed with success"), item.logFile);
        } else {
            logMessage(LL_FATAL, QString("Before-restart command failed"), item.logFile);
            m_model->data()[index].restartError = true;
            m_model->data()[index].status = ProgramItem::Stopped;
            displayInfo();
            return;
        }
    }

    // Restart command
    QProcess proc;
    proc.setProgram(item.program);
    proc.setNativeArguments(item.arguments);
    proc.setWorkingDirectory(item.startIn);
    qint64 pid = 0;
    if (proc.startDetached(&pid)) {
        m_model->data()[index].pid = pid;
        logMessage(LL_INFO, QString("Restarted %1 with new pid: %2").arg(item.name).arg(pid), item.logFile);
        m_model->data()[index].restartCount++;
        m_model->data()[index].lastRestartDateTime = QDateTime::currentDateTime();
    } else {
        logMessage(LL_FATAL, QString("Couldn't restart %1").arg(item.name), item.logFile);
        if (m_enableNotifications)
            m_sysTrayIcon->showMessage(tr("ForeverUp"), tr("ForeverUp couldn't restart \"%1\"").arg(item.name),
                                       QIcon(":/icon/FUP_icon_white.png"), 5000);
        m_model->data()[index].restartError = true;
        m_model->data()[index].status = ProgramItem::Stopped;
        m_timerList.at(index)->stop();
    }

    displayInfo();
}

void MainWindow::onCloseRequest()
{
    m_enableNotifications = false;
    on_actionStopMonitoring_triggered();
    close();
    emit finished();
}

void MainWindow::onSystemTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        if (isHidden()) show();
        activateWindow();
        raise();
    }
}

void MainWindow::notifyBackground()
{
    if (m_bgNotified)
        return;
    m_bgNotified = true;
    if (m_enableNotifications)
        m_sysTrayIcon->showMessage(tr("ForeverUp"), tr("ForeverUp is still running in the background"),
                                   QIcon(":/icon/FUP_icon_white.png"), 1000);
}

void MainWindow::on_tblProgramList_customContextMenuRequested(const QPoint &pos)
{
    QItemSelectionModel *select = ui->tblProgramList->selectionModel();
    if (!select->hasSelection())
        return;
    m_contextMenu->exec(ui->tblProgramList->viewport()->mapToGlobal(pos));
}

void MainWindow::on_tblProgramList_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    displayInfo();
}

void MainWindow::on_tblProgramList_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    on_cmdEditProgram_clicked();
}

void MainWindow::on_actionQuit_triggered()
{
    onCloseRequest();
}

void MainWindow::on_cmdEditNameDescription_clicked()
{
    QItemSelectionModel *select = ui->tblProgramList->selectionModel();
    if (!select->hasSelection())
        return;
    int row = select->selectedRows().first().row();

    NameAndDescriptionDialog dialog(this);
    dialog.setWindowTitle(tr("Name and description - Edit"));
    dialog.setName(m_model->data()[row].name);
    dialog.setDescription(m_model->data()[row].description);
    if (dialog.exec() != QDialog::Accepted)
        return;

    m_model->data()[row].name = dialog.name();
    m_model->data()[row].description = dialog.description();
}

void MainWindow::displayInfo()
{
    QItemSelectionModel *select = ui->tblProgramList->selectionModel();
    if (!select->hasSelection())
        return;
    int row = select->selectedRows().first().row();
    ProgramItem item = m_model->data()[row];
    ui->txtProgram->setText(item.program);
    ui->txtProgramName->setText(item.program.mid(item.program.lastIndexOf("/") + 1));
    QIcon icon = m_iconProvider.icon(QFileInfo(item.program));
    ui->txtProgramIcon->setPixmap(icon.pixmap(ui->txtProgramIcon->size()));
    ui->txtRestartCount->setText(QString::number(item.restartCount));
    ui->txtLastRestartDateTime->setText(item.lastRestartDateTime.toString("yyyy/MM/dd HH:mm:ss"));
}

bool MainWindow::isProcessHanging(int pid)
{
    // Windows API
    // find all hWnds (vhWnds) associated with a process id (dwProcessID)
    QVector<HWND> hWnds;
    HWND hCurWnd = nullptr;
    do
    {
        hCurWnd = FindWindowEx(nullptr, hCurWnd, nullptr, nullptr);
        DWORD checkProcessID = 0;
        GetWindowThreadProcessId(hCurWnd, &checkProcessID);
        if (checkProcessID == (DWORD)pid)
        {
            hWnds.append(hCurWnd);  // add the found hCurWnd to the vector
        }
    }
    while (hCurWnd != nullptr);

    if (hWnds.isEmpty()) { // If no whnd, let's consider nothing happened
        qDebug() << "#NO WHNDS ASSOCIATED WITH PID:" << pid;
        return false;
    }

    qDebug() << "#WHNDS:" << hWnds << "ASSOCIATED WITH PID:" << pid;

    DWORDLONG Result = 0;
    LRESULT Return = 0;
    for (int i=0 ; i< hWnds.size() ; i++) {
        Return = ::SendMessageTimeout( hWnds.at(i), WM_NULL, 0, 0, SMTO_ABORTIFHUNG, 500, &Result);
        if (Return) break; // If Return isn't null,
        // the window processed the message and the process isn't hanging.
    }
    return !Return;
}

void MainWindow::logMessage(QString level, QString msg, QString fileName)
{
    QString line = QString("%1 [%2] %3").arg(QDateTime::currentDateTime().toString(Qt::ISODateWithMs), level, msg);
    qDebug() << line;
    ui->txtLog->appendHtml(line);

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (file.size() > 1024 * 1024 * 1024) {
        QFile::rename(fileName, QString(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss__%1").arg(fileName)));
    }
    if (!file.open(QIODevice::Append | QIODevice::WriteOnly))
        return;
    QTextStream ou(&file);
    ou << line << endl;
    file.close();
}

void MainWindow::on_actionEnableNotifications_triggered(bool checked)
{
    m_enableNotifications = checked;
}

void MainWindow::onCheckShowRequestTimeout()
{
    if (m_showRequest->lock()) {
        qint64 *data = (qint64*)m_showRequest->data();
        if (*data == qApp->applicationPid()) {
            qDebug() << "Show request from another process!";
            if (isHidden()) show();
            activateWindow();
            raise();
        }
        *data = 0;
        m_showRequest->unlock();
    } else {
        qDebug() << "showReq: Couldn't lock shared memory.";
    }
}

void MainWindow::on_actionAboutForeverUp_triggered()
{
    m_about->exec();
}

void MainWindow::on_actionAdvanced_triggered()
{
    if (m_settings->exec()) {
        m_settings->saveSettings();
        applyAdvancedSettings();
    } else {
        m_settings->undo();
    }
}

void MainWindow::applyAdvancedSettings()
{
    QString startupPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + "/Startup";
    QString linkFileName = QString("%1/ForeverUp.lnk").arg(startupPath);

    if (QFile::exists(linkFileName)) {
        qDebug() << "Removing old shortcut:" << QFile::remove(linkFileName);
    }

    if (m_settings->launchAtStartupEnabled()) {
        QStringList arguments;
        if (m_settings->isQuiet()) arguments << "quiet";
        if (m_settings->isNoGui()) arguments << "nogui";
        if (m_settings->isAutostart()) arguments << "autostart";

        QString target = QDir::toNativeSeparators(qApp->applicationFilePath());
        QString link = QDir::toNativeSeparators(linkFileName);
        QString description = tr("Launch ForeverUp at startup");
        int showWindow = SW_SHOWNORMAL;
        QString execDir = QDir::toNativeSeparators(qApp->applicationDirPath());

        int ret = ShortcutProvider::create(target.toUtf8().data(), arguments.join(" ").toUtf8().data(),
                                           link.toUtf8().data(), description.toUtf8().data(),
                                           showWindow, execDir.toUtf8().data(), target.toUtf8().data(), 0);
        if (ret < 0) {
            qDebug() << "Couldn't create shortcut:" << linkFileName;
            QMessageBox::warning(this, tr("Couldn't create shortcut"),
                                 tr("Launch at startup will be reset to disabled for now."));
            m_settings->setLaunchAtStartupEnabled(false);
        }
    }
}
