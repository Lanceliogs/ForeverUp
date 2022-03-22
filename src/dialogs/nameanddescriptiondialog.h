#ifndef NAMEANDDESCRIPTIONDIALOG_H
#define NAMEANDDESCRIPTIONDIALOG_H

#include <QDialog>

namespace Ui {
class NameAndDescriptionDialog;
}

class NameAndDescriptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NameAndDescriptionDialog(QWidget *parent = nullptr);
    ~NameAndDescriptionDialog();

    void setName(QString text);
    QString name() const;

    void setDescription(QString text);
    QString description() const;

private:
    Ui::NameAndDescriptionDialog *ui;

};

#endif // NAMEANDDESCRIPTIONDIALOG_H
