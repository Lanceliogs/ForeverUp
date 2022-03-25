#include "advancedsettingsdialog.h"
#include "ui_advancedsettingsdialog.h"

AdvancedSettingsDialog::AdvancedSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdvancedSettingsDialog)
{
    ui->setupUi(this);

    loadSettings();
}

AdvancedSettingsDialog::~AdvancedSettingsDialog()
{
    saveSettings();
    delete ui;
}

int AdvancedSettingsDialog::exec()
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

void AdvancedSettingsDialog::undo()
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

void AdvancedSettingsDialog::loadSettings()
{
    QString setFileName = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/foreverup.ini";
    QSettings set(setFileName, QSettings::IniFormat);
    set.setIniCodec("UTF-8");

    setLaunchAtStartupEnabled(set.value("launchAtStartup", false).toBool());
    setQuiet(set.value("quiet", false).toBool());
    setNoGui(set.value("nogui", false).toBool());
    setAutostart(set.value("autostart", false).toBool());
}

void AdvancedSettingsDialog::saveSettings()
{
    QString setFileName = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/foreverup.ini";
    QSettings set(setFileName, QSettings::IniFormat);
    set.setIniCodec("UTF-8");

    set.setValue("launchAtStartup", launchAtStartupEnabled());
    set.setValue("quiet", isQuiet());
    set.setValue("nogui", isNoGui());
    set.setValue("autostart", isAutostart());
}

void AdvancedSettingsDialog::setLaunchAtStartupEnabled(bool enabled)
{
    ui->grpLaunchAtStartup->setChecked(enabled);
}

bool AdvancedSettingsDialog::launchAtStartupEnabled() const
{
    return ui->grpLaunchAtStartup->isChecked();
}

void AdvancedSettingsDialog::setQuiet(bool enabled)
{
    ui->chkIsQuiet->setChecked(enabled);
}

bool AdvancedSettingsDialog::isQuiet() const
{
    return ui->chkIsQuiet->isChecked();
}

void AdvancedSettingsDialog::setNoGui(bool enabled)
{
    ui->chkNoGui->setChecked(enabled);
}

bool AdvancedSettingsDialog::isNoGui() const
{
    return ui->chkNoGui->isChecked();
}

void AdvancedSettingsDialog::setAutostart(bool enabled)
{
    ui->chkAutostart->setChecked(enabled);
}

bool AdvancedSettingsDialog::isAutostart() const
{
    return ui->chkAutostart->isChecked();
}
