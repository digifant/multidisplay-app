#include <QCloseEvent>
#include <QtCore/qmath.h>

#include "AndroidMainWindow.h"
#include "ui_AndroidMainWindow.h"
#include "AppEngine.h"

AndroidMainWindow::AndroidMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AndroidMainWindow)
{
    ui->setupUi(this);

//    resize(350,768);

    l = new QGridLayout(this);
    ui->centralwidget->setLayout(l);
    boostw = new MeasurementWidget (this, "Boost", 0, 2.0);
    boostw->setWideMode(false);
    boostw->setDigits(4);
    boostw->resize(100,100);
    l->addWidget(boostw, 0, 0);
    l->setRowStretch(0, 1);
    l->setColumnStretch(0, 1);

    lw = new MeasurementWidget ( this, QString("Lambda"), 0.69, 1.36);
    lw->setWideMode(true);
    lw->setDigits(3.5);
    l->addWidget(lw, 1, 0);
    l->setRowStretch(1, 1);

    egtw = new MaxEgtWidget ( this, QString("EGT"), 750, 950);
    egtw->setWideMode(true);
    egtw->setDigits(4);
    egtw->setGeometry( QRect(100,100,100,100) );
    l->addWidget(egtw, 2,0);
    l->setRowStretch(2, 1);

    dfexw = new DFExtendedWidget (this, "Digifant 1");
//    l->addWidget(dfexw, 0,1, 4, 1);
     l->addWidget(dfexw, 3,0);
     l->setRowStretch(3, 3);

     l->setHorizontalSpacing(0);
     l->setVerticalSpacing(0);

//    QVBoxLayout *v = new QVBoxLayout (this);
//    boostw = new MeasurementWidget (this, "Boost", 0, 2.0);
//    boostw->setWideMode(false);
//    boostw->setDigits(4);
////    boostw->resize(1000,1000);
//    v->addWidget(boostw);
//    ui->centralwidget->setLayout(v);

     t = QTime::currentTime();
     t.start();
}

AndroidMainWindow::~AndroidMainWindow()
{
    delete ui;
}

void AndroidMainWindow::closeEvent(QCloseEvent *event) {
    emit writeSettings();
    event->accept();
    QMainWindow::closeEvent ( event );
}


void AndroidMainWindow::visualize (MdDataRecord *d) {
    if ( isVisible() && d->getSensorR() != NULL ) {

        if ( t.elapsed() > 100 ) {
            t.restart();

            lw->setValue(  d->getSensorR()->getLambda() );
            QMap<QString, double> e = d->getSensorR()->getHighestEgt();
            egtw->setValue( e["temp"], (quint8) e["idx"]  );

            if ( dfexw )
                dfexw->setValue( d );

            double boostKpa = AppEngine::getInstance()->getDfBoostTransferFunction()->map( d->getSensorR()->df_boost_raw ) ;
            boostw->setValue( qFloor ( boostKpa ) / 100.0 - 1.0 );
        }
    }
}
