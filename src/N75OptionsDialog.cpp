#include "N75OptionsDialog.h"
#include "ui_N75OptionsDialog.h"

N75OptionsDialog::N75OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::N75OptionsDialog)
{
    ui->setupUi(this);
}

N75OptionsDialog::~N75OptionsDialog()
{
    delete ui;
}
