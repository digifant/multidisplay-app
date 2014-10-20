#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
//    ui->webView->load (QUrl (QString("http://code.google.com/p/multidisplay/")));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
