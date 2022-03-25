#include "aboutwidget.h"
#include "ui_aboutwidget.h"

AboutWidget::AboutWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutWidget)
{
    ui->setupUi(this);
    layout()->setSizeConstraint( QLayout::SetFixedSize );

#ifdef GIT_COMMIT
    QString glorifiedName = BUILD_USERNAME;
    if (!glorifiedName.isEmpty())
        glorifiedName[0] = glorifiedName[0].toUpper();

    QString versionWithUrl = QString("<html><head/><body><p>"
"<a href=\"https://github.com/Lanceliogs/ForeverUp/commit/%1\">"
"<span style=\" text-decoration: underline; color:#0000ff;\">"
"%2"
"</span></a></p></body></html>").arg(QString(GIT_COMMIT).right(7), GIT_COMMIT);

    ui->txtVersion->setText(versionWithUrl);
    ui->txtUsername->setText(glorifiedName);
    ui->txtBuildDateTime->setText(BUILD_DATETIME);
#else
    ui->txtVersion->setText(tr("Unknown"));
    ui->txtUsername->setText(tr("Unknown");
            ui->txtBuildDateTime->setText(tr("Unknown");
        #endif
}

AboutWidget::~AboutWidget()
{
    delete ui;
}
