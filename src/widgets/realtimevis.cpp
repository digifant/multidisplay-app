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
    //dashboard realtimevis on androidmainwindow
    QMainWindow *amw = qobject_cast<QMainWindow*>(parent);
    if ( amw != nullptr )
        //amw->centralWidget()->setLayout(pl);
        amw->setCentralWidget(this);
    else {
        parent->setLayout(pl);
        pl->addWidget(this);
    }

    QHBoxLayout *h = new QHBoxLayout();
    h->setContentsMargins(0,0,0,0);
    h->setSpacing(0);
    this->setLayout(h);

#if !defined (Q_WS_MAEMO_5)  && !defined (ANDROID)  && !defined (Q_OS_IOS)
    bg1 = new BoostBarGraphWidget (this);
//    bg2 = new LambdaBarGraphWidget (this);
    bg2 = NULL;
    bg3 = new RPMBarGraphWidget (this, "RPM");
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

    QFrame *fWidgets1 = new QFrame (this);
    fWidgets1->setContentsMargins(0,0,0,0);
    QVBoxLayout *v = new QVBoxLayout();
    //disable margin around the vbox layout
    v->setContentsMargins(0,0,0,0);
    v->setSpacing(0);
    fWidgets1->setLayout(v);
#if defined (DIGIFANTAPP)
    h->addWidget(fWidgets1, 1);
#else
    h->addWidget(fWidgets1, 2);
#endif
#if defined (DIGIFANTVANAPP)
    boostW = new MeasurementWidget ( this, QString("AFM (Volts)"), 0.01, 2.5, 4.5, Qt::green, Qt::green, Qt::red);
    boostW->setDigits(4);
    boostW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(boostW);

    lambdaW = new MeasurementWidget ( this, QString("WbLambda"), 0.90, 1.0, 1.10, Qt::red, Qt::green, Qt::red);
    lambdaW->setDigits(4);
    lambdaW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(lambdaW);

    egtW = new MaxEgtWidget ( this, QString("RPM") );
    egtW->setDigits(4);
    egtW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(egtW);
#else

    boostW = new MeasurementWidget ( this, QString("Boost [bar]"), -1, 0, 2.0, Qt::darkGreen, Qt::green, Qt::red);
    boostW->setDigits(4);
    boostW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(boostW);

    lambdaW = new MeasurementWidget ( this, QString("WB Lambda"), 0.85, 1.1, 1.36, Qt::green, Qt::yellow, Qt::red);
    lambdaW->setDigits(3.8);
    lambdaW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(lambdaW);

    egtW = new MaxEgtWidget ( this, QString("EGT") );
    egtW->setDigits(4);
    egtW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(egtW);
#endif
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

    switchEcu();

    timer.start();

    //testOverlay = new Overlay (this);

    topOverlay = new MessageOverlay (this, Qt::AlignJustify);
    //topOverlay->showMessage (QString("Test-Overlay Message"), 10);
    middleOverlay = new MessageOverlay (this, Qt::AlignVCenter | Qt::AlignHCenter);
    //middleOverlay->showMessage (QString("Test-Overlay Message"), 10);
    bottomOverlay = new MessageOverlay (this, Qt::AlignBottom | Qt::AlignHCenter);
    //bottomOverlay->showMessage (QString("Test-Overlay Message"), 10);
}

void RealTimeVis::possibleCfgChange () {
    switchEcu();
}

void RealTimeVis::showMessage(const QString &msg, const int forSeconds)
{
    if ( topOverlay )
        topOverlay->showMessage(msg,forSeconds);
}

void RealTimeVis::showStatusMessage(const QString &msg)
{
    if ( topOverlay )
        topOverlay->showMessage(msg,5);
}

void RealTimeVis::showMessage3(const QString &msg)
{
    if ( topOverlay )
        topOverlay->showMessage(msg,3);
}

void RealTimeVis::hideMessage()
{
    if (topOverlay)
        topOverlay->hideMessage();
}

void RealTimeVis::visualize (MdDataRecord *d) {
    if ( AppEngine::getInstance()->getActualizeDashboard() && d->getSensorR() != NULL ) {
#if defined (Q_OS_IOS)
        //100ms causes 100% cpu load on IOS
        if ( timer.elapsed() > 500 ) {
#else
        if ( timer.elapsed() > 100 ) {
#endif
            timer.restart();

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
#if defined (DIGIFANTVANAPP)
            // only works for afm if boost sensor set to 100kpa
            // casted to 2 decimal places in transfer function
            // had it like below until I figured out issue with missing code in MdBinaryProtocol line 489
            //boostW->setValue(AppEngine::getInstance()->getDfBoostTransferFunction()->map( d->getSensorR()->df_boost_raw ));
            boostW->setValue(qFloor( d->getSensorR()->getBoost()*100)/100.0 );  // us qfloor to set to 2 digits must use 100.0 for divide
#else
            boostW->setValue(  d->getSensorR()->getBoost() );
#endif
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
        boostW->setVisible(true);
        rpmW->setVisible(true);
    }
    if ( ecuStr == "VR6" ) {
        fDfWidget->setVisible(false);
        fVr6Widget->setVisible(true);
        boostW->setVisible(true);
        rpmW->setVisible(true);
    }
    if ( ecuStr == "generic (WB Lambda only)" ) {
        fDfWidget->setVisible(false);
        fVr6Widget->setVisible(false);
        boostW->setVisible(false);
        rpmW->setVisible(false);
    }

}

bool RealTimeVis::mdMode()
{
    QSettings settings;
    bool mdMode = settings.value("md/md", QVariant (MDMODE)).toBool();
#if defined (DIGIFANTVANAPP)
    //fix: was true for digifantvanapp ?!
    mdMode = false;
#endif
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
