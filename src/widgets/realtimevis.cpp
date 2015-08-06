#include "realtimevis.h"
#include "rtwidget.h"
#include "MdData.h"
#include "AppEngine.h"
#include "DFExtendedWidget.h"
#include <widgets/VR6Widget.h>

#include <QVBoxLayout>
#include <QTime>
#include <QSettings>


RealTimeVis::RealTimeVis(QWidget *parent):
    QWidget(parent)
{
    QHBoxLayout *pl = new QHBoxLayout();
    pl->setContentsMargins(0,0,0,0);
    pl->setSpacing(0);
    parent->setLayout(pl);
    pl->addWidget(this);

    QHBoxLayout *h = new QHBoxLayout();
    h->setContentsMargins(0,0,0,0);
    h->setSpacing(0);
    this->setLayout(h);

#if !defined (Q_WS_MAEMO_5)  && !defined (ANDROID)
    bg1 = new BoostBarGraphWidget (this);
//    bg2 = new LambdaBarGraphWidget (this);
    bg2 = NULL;
    bg3 = new RPMBarGraphWidget (this);
    h->addWidget(bg1, 1);
//    h->addWidget(bg2, 1);
    h->addWidget(bg3, 1);
#else
    bg1=NULL;
    bg2=NULL;
    bg3=NULL;
#endif

    //Digifant 1 widget
//        QFrame *fDfWidget = new QFrame (this);
    fDfWidget = new QFrame (this);
    fDfWidget->setContentsMargins(0,0,0,0);
    QVBoxLayout *v2 = new QVBoxLayout();
    //disable margin around the vbox layout
    v2->setContentsMargins(0,0,0,0);
    v2->setSpacing(0);
    fDfWidget->setLayout(v2);
    h->addWidget(fDfWidget, 3);
    dfexW = new DFExtendedWidget (this, "Digifant I");
    v2->addWidget(dfexW);

    //VR6 widget
    fVr6Widget = new QFrame (this);
    fVr6Widget->setContentsMargins(0,0,0,0);
    QVBoxLayout *v4 = new QVBoxLayout();
    //disable margin around the vbox layout
    v4->setContentsMargins(0,0,0,0);
    v4->setSpacing(0);
    fVr6Widget->setLayout(v4);
    h->addWidget(fVr6Widget, 3);
    vr6W = new VR6Widget (this, "VR6 M3.8.1");
    v4->addWidget(vr6W);


    switchEcu();


    QFrame *fWidgets1 = new QFrame (this);
    fWidgets1->setContentsMargins(0,0,0,0);
    QVBoxLayout *v = new QVBoxLayout();
    //disable margin around the vbox layout
    v->setContentsMargins(0,0,0,0);
    v->setSpacing(0);
    fWidgets1->setLayout(v);
    h->addWidget(fWidgets1, 2);

    boostW = new MeasurementWidget ( this, QString("Boost [bar]"), -1, 0, 2.0, Qt::darkGreen, Qt::green, Qt::red);
    boostW->setDigits(4);
    boostW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(boostW);

    lambdaW = new MeasurementWidget ( this, QString("Lambda"), 0.85, 1.1, 1.36, Qt::green, Qt::yellow, Qt::red);
    lambdaW->setDigits(3.8);
    lambdaW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(lambdaW);

    egtW = new MaxEgtWidget ( this, QString("EGT") );
    egtW->setDigits(4);
    egtW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(egtW);

    bexW = new BoostExtendedWidget ( this, QString("N75 debug") );
    bexW->setDigits(12);
    bexW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(bexW);


    QFrame *fWidgets2 = new QFrame (this);
    fWidgets2->setContentsMargins(0,0,0,0);
    QVBoxLayout *v3 = new QVBoxLayout();
    //disable margin around the vbox layout
    v3->setContentsMargins(0,0,0,0);
    v3->setSpacing(0);
    fWidgets2->setLayout(v3);
    h->addWidget(fWidgets2, 1);
    efrW = new EFRWidget ( this, QString("EFR") );
    v3->addWidget(efrW);
    oilW = new PressureWidget (this, "Oil Pres", 0.2, 0.5, 1, Qt::red, Qt::yellow, Qt::green);
    v3->addWidget(oilW);
    fuelW = new FuelPressureWidget (this, "Fuel Pres");
    v3->addWidget(fuelW);
    rpmW = new MeasurementWidget (this, "RPM", 0,7200,7500, QColor(Qt::green), QColor(Qt::green), QColor(Qt::red));
    if ( mdMode() )
        v3->addWidget(rpmW);
    else {
        v->addWidget(rpmW);
        fWidgets2->hide();
    }

    t = QTime::currentTime();
    t.start();
}

void RealTimeVis::possibleCfgChange () {
    switchEcu();
}

void RealTimeVis::visualize (MdDataRecord *d) {
    if ( AppEngine::getInstance()->getActualizeDashboard() && d->getSensorR() != NULL ) {

        if ( t.elapsed() > 100 ) {
            t.restart();

            //TODO encapsulate in objects

            if ( bg1 != NULL ) {
                bg1->setValue( d->getSensorR()->getBoost() );
            }
            if ( bg2 != NULL ) {
                LambdaBarGraphWidget* l = qobject_cast<LambdaBarGraphWidget*>(bg2);
                if ( d->getSensorR()->getThrottle() >= 90 && l )
                    l->wotOn();
                else
                    l->wotOff();
                bg2->setValue( d->getSensorR()->getLambda() );
            }

            if ( bg3 != NULL ) {
                bg3->setValue( d->getSensorR()->getRpm() );
            }

            boostW->setValue(  d->getSensorR()->getBoost() );
            lambdaW->setValue(  d->getSensorR()->getLambda() );
            QMap<QString, double> e = d->getSensorR()->getHighestEgt();
            egtW->setValue( e["temp"], (quint8) e["idx"]  );

            bexW->setValue( d );

//            dfw->setValue( 0, d->getSensorR()->df_flags, d->getSensorR()->df_iat, d->getSensorR()->df_ect,
//                          d->getSensorR()->df_ignition, d->getSensorR()->df_ignition_total_retard,
//                          d->getSensorR()->df_inj_time, d->getSensorR()->df_voltage_raw);

            if ( dfexW )
                dfexW->setValue( d );

            if ( efrW )
                efrW->setValue( d->getSensorR()->efr_speed );

            if ( oilW )
                oilW->setValue( d->getSensorR()->getVDOPres3() );
            if ( fuelW )
                fuelW->setValue( d->getSensorR()->getVDOPres2(), d->getSensorR()->getBoost() );

            if ( rpmW )
                rpmW->setValue( d->getSensorR()->getRpm() );
        }
    }
}

void RealTimeVis::paintEvent(QPaintEvent *event) {
    //    qDebug() << "RealTimeVis::paintEvent" << endl;
}

bool RealTimeVis::event(QEvent *e)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if ( e->type() == QEvent::OrientationChange ) {
        qDebug() << "RealTimeVis::event: orientation change ";
    }
#endif
    return QWidget::event(e);
}

void RealTimeVis::resizeEvent(QResizeEvent *event)
{
    qDebug() << "RealTimeVis::resizeEvent width=" << event->size().width() << " height=" << event->size().height();
}

void RealTimeVis::switchEcu()
{
    QSettings settings;
    QString ecuStr = settings.value("md/ecu", QVariant (QString("Digifant 1"))).toString();
    if ( ecuStr == "Digifant 1" ) {
        fDfWidget->setVisible(true);
        fVr6Widget->setVisible(false);
    } else if ( ecuStr == "VR6 M3.8.1" ) {
        fDfWidget->setVisible(false);
        fVr6Widget->setVisible(true);
    } else {
        fDfWidget->setVisible(false);
        fVr6Widget->setVisible(false);
    }

}

bool RealTimeVis::mdMode()
{
    QSettings settings;
    bool mdMode = settings.value("md/md", QVariant (true)).toBool();
    if ( mdMode ) {
        egtW->show();
        bexW->show();

        efrW->show();
        oilW->show();
        fuelW->show();
    } else {
        egtW->hide();
        bexW->hide();

        efrW->hide();
        oilW->hide();
        fuelW->hide();
    }
    return mdMode;
}
