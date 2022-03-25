#ifndef ADVANCEDSETTINGSDIALOG_H
#define ADVANCEDSETTINGSDIALOG_H

#include <QLineEdit>
#include <QAbstractButton>
#include <QAbstractSpinBox>
#include <QComboBox>
#include <QSettings>
#include <QStandardPaths>
#include <QDialog>

namespace Ui {
class AdvancedSettingsDialog;
}

class AdvancedSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdvancedSettingsDialog(QWidget *parent = nullptr);
    ~AdvancedSettingsDialog();

    int exec();
    void undo();

    void loadSettings();
    void saveSettings();

    void setLaunchAtStartupEnabled(bool enabled);
    bool launchAtStartupEnabled() const;

    void setQuiet(bool enabled);
    bool isQuiet() const;

    void setNoGui(bool enabled);
    bool isNoGui() const;

    void setAutostart(bool enabled);
    bool isAutostart() const;

private:
    Ui::AdvancedSettingsDialog *ui;

    QList<QString> m_storedTexts;
    QList<int> m_storedValues;
    QList<bool> m_storedChecks;
    QList<QString> m_storedCombos;
};

#endif // ADVANCEDSETTINGSDIALOG_H
