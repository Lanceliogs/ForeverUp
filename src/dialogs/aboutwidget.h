#ifndef ABOUTWIDGET_H
#define ABOUTWIDGET_H

#if __has_include("../git_config.h")
#include "../git_config.h"
#endif

#include <QDialog>
#include <QDesktopWidget>

namespace Ui {
class AboutWidget;
}

class AboutWidget : public QDialog
{
    Q_OBJECT

public:
    explicit AboutWidget(QWidget *parent = 0);
    ~AboutWidget();

private:
    Ui::AboutWidget *ui;
};

#endif // ABOUTWIDGET_H
