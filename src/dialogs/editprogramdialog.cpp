#include "editprogramdialog.h"
#include "ui_editprogramdialog.h"

EditProgramDialog::EditProgramDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditProgramDialog)
{
    ui->setupUi(this);
}

EditProgramDialog::~EditProgramDialog()
{
    delete ui;
}

int EditProgramDialog::exec()
{
    // Store settings to undo
    m_storedTexts.clear();
    m_storedValues.clear();
    m_storedChecks.clear();
    m_storedCombos.clear();

    foreach (QLineEdit* le, this->findChildren<QLineEdit*>()) {
        m_storedTexts.append(le->text());
    }
    foreach (QAbstractSpinBox* sb, this->findChildren<QAbstractSpinBox*>()) {
        m_storedValues.append(sb->property("value").toDouble());
    }
    foreach (QAbstractButton* cb, this->findChildren<QAbstractButton*>()) {
        m_storedChecks.append(cb->isChecked());
    }
    foreach (QComboBox* cb, this->findChildren<QComboBox*>()) {
        m_storedCombos.append(cb->currentText());
    }

    return QDialog::exec();
}

void EditProgramDialog::undo()
{
    foreach (QLineEdit* le, this->findChildren<QLineEdit*>()) {
        le->setText(m_storedTexts.takeFirst());
    }
    foreach (QAbstractSpinBox* sb, this->findChildren<QAbstractSpinBox*>()) {
        sb->setProperty("value", m_storedValues.takeFirst());
    }
    foreach (QAbstractButton* cb, this->findChildren<QAbstractButton*>()) {
        cb->setChecked(m_storedChecks.takeFirst());
    }
    foreach (QComboBox* cb, this->findChildren<QComboBox*>()) {
        cb->setCurrentText(m_storedCombos.takeFirst());
    }
}

void EditProgramDialog::setProgram(QString text)
{
    ui->txtExePath->setText(text);
}

QString EditProgramDialog::program() const
{
    return ui->txtExePath->text();
}

void EditProgramDialog::setArguments(QString text)
{
    ui->txtArguments->setText(text);
}

QString EditProgramDialog::arguments() const
{
    return ui->txtArguments->text();
}

void EditProgramDialog::setStartIn(QString text)
{
    ui->txtStartIn->setText(text);
}

QString EditProgramDialog::startIn() const
{
    return ui->txtStartIn->text();
}

void EditProgramDialog::setCheckInterval(int minutes)
{
    ui->txtCheckInterval->setValue(minutes);
}

int EditProgramDialog::checkInterval() const
{
    return ui->txtCheckInterval->value();
}

void EditProgramDialog::setMemoryLimit(int mo)
{
    ui->txtMaxMemory->setValue(mo);
}

int EditProgramDialog::memoryLimit() const
{
    return ui->txtMaxMemory->value();
}

void EditProgramDialog::setMaxHangingCount(int value)
{
    ui->txtMaxHangingCount->setValue(value);
}

int EditProgramDialog::maxHangingCount() const
{
    return ui->txtMaxHangingCount->value();
}

void EditProgramDialog::setLogFile(QString text)
{
    ui->txtLogFile->setText(text);
}

QString EditProgramDialog::logFile() const
{
    return ui->txtLogFile->text();
}

void EditProgramDialog::setScriptBeforeRestart(QString text)
{
    ui->txtScriptBeforeRestartPath->setText(text);
}

QString EditProgramDialog::scriptBeforeRestart() const
{
    return ui->txtScriptBeforeRestartPath->text();
}

void EditProgramDialog::setIsItemEnabled(bool enabled)
{
    ui->chkEnabled->setChecked(enabled);
}

bool EditProgramDialog::isItemEnabled() const
{
    return ui->chkEnabled->isChecked();
}

void EditProgramDialog::on_cmdBrowseExePath_clicked()
{
    QString dirPath = ui->txtExePath->text().mid(0, ui->txtExePath->text().lastIndexOf("/"));
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select the executable"), dirPath, tr("Executable file (*.exe)"));
    if (fileName.isEmpty())
        return;
    ui->txtExePath->setText(fileName);
}

void EditProgramDialog::on_cmdBrowseStartIn_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select the directory"), ui->txtStartIn->text());
    if (dirPath.isEmpty())
        return;
    ui->txtStartIn->setText(dirPath);
}

void EditProgramDialog::on_cmdBrowseLogFile_clicked()
{
    QString dirPath = ui->txtStartIn->text();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select the log file"), dirPath, tr("Log file (*.log *.txt)"));
    if (fileName.isEmpty())
        return;
    ui->txtLogFile->setText(fileName);
}

void EditProgramDialog::on_cmdBrowseScriptBeforeRestart_clicked()
{
    QString dirPath = ui->txtScriptBeforeRestartPath->text().mid(0, ui->txtScriptBeforeRestartPath->text().lastIndexOf("/"));
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select the script"), dirPath, tr("Script (*.bat *.py *.cmd *.sh);;All files (*.*)"));
    if (fileName.isEmpty())
        return;
    ui->txtScriptBeforeRestartPath->setText(fileName);
}
