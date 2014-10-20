#include "DigifantApplicationWindow.h"
#include "widgets/rtwidget.h"
#include "widgets/DFExtendedWidget.h"
#include <QtCore/qmath.h>
#include "AppEngine.h"

DigifantApplicationWindow::DigifantApplicationWindow(QWidget *parent) :
    QDialog(parent)
{
    resize(350,768);

    l = new QGridLayout(this);

    boostw = new MeasurementWidget ( this, QString("Boost [bar]"), -1, 0, 2.0, Qt::darkGreen, Qt::green, Qt::red);
    boostw->setLowHeigth(true);
    boostw->setDigits(4);
    boostw->resize(100,100);
    l->addWidget(boostw, 0, 0);
    l->setRowStretch(0, 1);

    lw = new MeasurementWidget ( this, QString("Lambda"), 0.85, 1.1, 1.36, Qt::green, Qt::yellow, Qt::red);
    lw->setLowHeigth(true);
    lw->setDigits(3.5);
    l->addWidget(lw, 1, 0);
    l->setRowStretch(1, 1);

    egtw = new MaxEgtWidget ( this, QString("EGT") );
    egtw->setLowHeigth(true);
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

     t = QTime::currentTime();
     t.start();
}


void DigifantApplicationWindow::visualize (MdDataRecord *d) {
    if ( isVisible() && d->getSensorR() != NULL ) {

        if ( t.elapsed() > 100 ) {
            t.restart();

            lw->setValue(  d->getSensorR()->getLambda() );
            QMap<QString, double> e = d->getSensorR()->getHighestEgt();
            egtw->setValue( e["temp"], (quint8) e["idx"]  );

//            bexw->setValue( d );

            if ( dfexw )
                dfexw->setValue( d );

//            boostw->setValue( (AppEngine::getInstance()->getDfBoostTransferFunction()->map( d->getSensorR()->df_boost_raw ) / 100) - 1.0  );

//            boostw->setValue( qFloor ( ((AppEngine::getInstance()->getDfBoostTransferFunction()->map( d->getSensorR()->df_boost_raw ) / 100) - 1.0) * 100) / 100  );

            double boostKpa = AppEngine::getInstance()->getDfBoostTransferFunction()->map( d->getSensorR()->df_boost_raw ) ;
            boostw->setValue( qFloor ( boostKpa ) / 100.0 - 1.0 );
        }
    }
}
