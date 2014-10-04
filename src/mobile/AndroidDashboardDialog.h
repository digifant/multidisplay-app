#ifndef ANDROIDDASHBOARDDIALOG_H
#define ANDROIDDASHBOARDDIALOG_H

#include <QDialog>

namespace Ui {
class AndroidDashboardDialog;
}

class AndroidDashboardDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AndroidDashboardDialog(QWidget *parent = 0);
    ~AndroidDashboardDialog();

private:
    Ui::AndroidDashboardDialog *ui;
};

#endif // ANDROIDDASHBOARDDIALOG_H
