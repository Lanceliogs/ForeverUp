#ifndef EDITPROGRAMDIALOG_H
#define EDITPROGRAMDIALOG_H

#include <QLineEdit>
#include <QAbstractButton>
#include <QAbstractSpinBox>
#include <QComboBox>
#include <QDialog>
#include <QFileDialog>

namespace Ui {
class EditProgramDialog;
}

class EditProgramDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditProgramDialog(QWidget *parent = nullptr);
    ~EditProgramDialog();

    int exec();
    void undo();

    void setProgram(QString text);
    QString program() const;

    void setArguments(QString text);
    QString arguments() const;

    void setStartIn(QString text);
    QString startIn() const;

    void setCheckInterval(int minutes);
    int checkInterval() const;

    void setMemoryLimit(int mo);
    int memoryLimit() const;

    void setMaxHangingCount(int value);
    int maxHangingCount() const;

    void setLogFile(QString text);
    QString logFile() const;

    void setScriptBeforeRestart(QString text);
    QString scriptBeforeRestart() const;

    void setIsItemEnabled(bool enabled);
    bool isItemEnabled() const;

private slots:
    void on_cmdBrowseExePath_clicked();

    void on_cmdBrowseStartIn_clicked();

    void on_cmdBrowseLogFile_clicked();

    void on_cmdBrowseScriptBeforeRestart_clicked();

private:
    Ui::EditProgramDialog *ui;

    QList<QString> m_storedTexts;
    QList<int> m_storedValues;
    QList<bool> m_storedChecks;
    QList<QString> m_storedCombos;
};

#endif // EDITPROGRAMDIALOG_H
