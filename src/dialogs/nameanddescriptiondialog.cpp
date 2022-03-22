#include "nameanddescriptiondialog.h"
#include "ui_nameanddescriptiondialog.h"

NameAndDescriptionDialog::NameAndDescriptionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NameAndDescriptionDialog)
{
    ui->setupUi(this);
}

NameAndDescriptionDialog::~NameAndDescriptionDialog()
{
    delete ui;
}

void NameAndDescriptionDialog::setName(QString text)
{
    ui->txtName->setText(text);
}

QString NameAndDescriptionDialog::name() const
{
    return ui->txtName->text().trimmed();
}

void NameAndDescriptionDialog::setDescription(QString text)
{
    ui->txtDescription->setPlainText(text);
}

QString NameAndDescriptionDialog::description() const
{
    return ui->txtDescription->toPlainText().replace("\n", " - ");
}
