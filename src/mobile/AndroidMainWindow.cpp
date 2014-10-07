#include <QCloseEvent>
#include <QtCore/qmath.h>
#include <QDebug>

#include "AndroidMainWindow.h"
#include "ui_AndroidMainWindow.h"
#include "AppEngine.h"



AndroidMainWindow::AndroidMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AndroidMainWindow)
{
    ui->setupUi(this);


     t = QTime::currentTime();
     t.start();
}

AndroidMainWindow::~AndroidMainWindow()
{
    delete ui;
}

void AndroidMainWindow::closeEvent(QCloseEvent *event) {
    //segfaults :(
    emit writeSettings();
    event->accept();
    QMainWindow::closeEvent ( event );
}

void AndroidMainWindow::resizeEvent(QResizeEvent *event)
{
    qDebug() << "AndroidMainWindow::resizeEvent size=" << event->size() << " old=" << event->oldSize();
    QMainWindow::resizeEvent(event);
}

void AndroidMainWindow::showStatusMessage(const QString &msg)
{
    qDebug() << "showStatusMessage";
    if ( ui->textEdit )
        ui->textEdit->insertPlainText( msg + "\n" );
}

void AndroidMainWindow::btPortClosed()
{
    ui->actionBluetoothToggleState->setText("Bluetooth connect");
}

void AndroidMainWindow::btPortOpened()
{
    ui->actionBluetoothToggleState->setText("Bluetooth disconnect");
}


