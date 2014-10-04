#include "AndroidDashboardDialog.h"
#include "ui_AndroidDashboardDialog.h"

AndroidDashboardDialog::AndroidDashboardDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AndroidDashboardDialog)
{
    ui->setupUi(this);
}

AndroidDashboardDialog::~AndroidDashboardDialog()
{
    delete ui;
}
