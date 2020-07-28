#include "AboutDialog.h"
#include "ui_AboutDialog.h"

#include <QDebug>
#include <QSettings>
#include <QPushButton>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

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
