#include "realtimevis.h"
#include "rtwidget.h"
#include "MdData.h"
#include "AppEngine.h"
#include "DFExtendedWidget.h"
#include <widgets/VR6Widget.h>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QTime>
#include <QSettings>


RealTimeVis::RealTimeVis(QWidget *parent):
    QWidget(parent)
{
#define REALTIMEVIS2021
#if defined REALTIMEVIS2021
    /* QGridLayout:
     * two or mo
     */
    //dashboard realtimevis on androidmainwindow
    QMainWindow *amw = qobject_cast<QMainWindow*>(parent);
    if ( amw != nullptr ) {
        amw->setCentralWidget(this);
    } else {
        QHBoxLayout *pl = new QHBoxLayout();
        pl->setContentsMargins(0,0,0,0);
        pl->setSpacing(0);
        parent->setLayout(pl);
        pl->addWidget(this);
    }
    gl = new QGridLayout();
    gl->setContentsMargins(0,0,0,0);
    gl->setSpacing(0);
    this->setLayout(gl);
    int colOffset = 0;
#if !defined (Q_WS_MAEMO_5)  && !defined (ANDROID)  && !defined (Q_OS_IOS)
    bg1 = new BoostBarGraphWidget (this);
    bg2 = nullptr;
    bg3 = new RPMBarGraphWidget (this, tr("RPM"));
    int rowSpan = 3;
    if ( isMultidisplayApp() )
        rowSpan = 4;
    gl->addWidget(bg1, 0,0, rowSpan, 1);
    gl->addWidget(bg3, 0, ++colOffset, rowSpan, 1);
    colOffset++;
#endif
    //ECU col
    dfexW = new DFExtendedWidget (this, tr("Digifant I"));
    if ( isDigifantApp() )
        gl->addWidget(dfexW, 0,colOffset,3,3);
    else
        gl->addWidget(dfexW, 0,colOffset,4,3);

    //2021-01-31: no more different ecu chooseable on android thanks to buggy Qt-5.15!
    //VR6 widget
    //vr6W = new VR6Widget (this, tr("VR6 M3.8.1"));
    //if ( isVR6Ecu() )
    //    gl->addWidget(vr6W, 0,colOffset,3,3);

    //2. col
#if defined (DIGIFANTVANAPP)
    boostW = new MeasurementWidget ( this, QString(tr("AFM (Volts)")), 0.01, 2.5, 4.5, Qt::green, Qt::green, Qt::red);
    boostW->setDigits(4);
    boostW->setGeometry( QRect(100,100,100,100) );
    if ( isDigifantApp() )
        gl->addWidget(boostW,0,colOffset+4);

    //lambdaW = new MeasurementWidget ( this, QString(tr("WbLambda")), 0.90, 1.0, 1.10, Qt::red, Qt::green, Qt::red);
    //extended widget: can be switched between NB and WB
    lambdaW = new LambdaExtWidget ( this );
    lambdaW->setDigits(4);
    lambdaW->setGeometry( QRect(100,100,100,100) );
    gl->addWidget(lambdaW,1,colOffset+4);

    egtW = new MaxEgtWidget ( this, QString(tr("RPM")) );
    egtW->setDigits(4);
    egtW->setGeometry( QRect(100,100,100,100) );
    if ( isMultidisplayApp() )
        gl->addWidget(egtW,2,colOffset+4);
#else

    boostW = new MeasurementWidget ( this, QString(tr("Boost [bar]")), -1, 0, 2.0, Qt::darkGreen, Qt::green, Qt::red);
    boostW->setDigits(4);
    boostW->setGeometry( QRect(100,100,100,100) );
    gl->addWidget(boostW,0,colOffset+4);

    //lambdaW = new MeasurementWidget ( this, QString(tr("WB Lambda")), 0.85, 1.1, 1.36, Qt::green, Qt::yellow, Qt::red);
    //extended widget: can be switched between NB and WB
    lambdaW = new LambdaExtWidget ( this );
    connect (lambdaW, SIGNAL(showStatusMessage(QString)), this, SLOT(triggerEmitShowStatusMessage(QString)));
    lambdaW->setDigits(3.8);
    lambdaW->setGeometry( QRect(100,100,100,100) );
    gl->addWidget(lambdaW,1,colOffset+4);

    egtW = new MaxEgtWidget ( this, QString("EGT") );
    egtW->setDigits(4);
    egtW->setGeometry( QRect(100,100,100,100) );
    if ( isMultidisplayApp() )
        gl->addWidget(egtW,2,colOffset+4);
#endif
    bexW = new BoostExtendedWidget ( this, QString(tr("N75")) );
    bexW->setDigits(12);
    bexW->setGeometry( QRect(100,100,100,100) );
    if ( isMultidisplayApp() )
        gl->addWidget(bexW,3,colOffset+4);

    //3. col (md app)
    efrW = new EFRWidget ( this, QString(tr("EFR")) );
    gl->addWidget(efrW,0,colOffset+5);
    oilW = new PressureWidget (this, tr("Oil Pres"), 0.2, 0.5, 1, Qt::red, Qt::yellow, Qt::green);
    gl->addWidget(oilW,1,colOffset+5);
    fuelW = new FuelPressureWidget (this, tr("Fuel Pres"));
    gl->addWidget(fuelW,2,colOffset+5);
    rpmW = new MeasurementWidget (this, tr("RPM"), 0,7200,7500, QColor(Qt::green), QColor(Qt::green), QColor(Qt::red));
    if ( mdMode() )
        gl->addWidget(rpmW,3,colOffset+5);
    else {
        gl->addWidget(rpmW,2,colOffset+4);
    }



#else
    QHBoxLayout *pl = new QHBoxLayout();
    pl->setContentsMargins(0,0,0,0);
    pl->setSpacing(0);
    //dashboard realtimevis on androidmainwindow
    QMainWindow *amw = qobject_cast<QMainWindow*>(parent);
    if ( amw != nullptr ) {
        //amw->centralWidget()->setLayout(pl);
        amw->setCentralWidget(this);
        delete (pl);
        pl = nullptr;
    } else {
        parent->setLayout(pl);
        pl->addWidget(this);
    }

    //global horizontal layout
    QHBoxLayout *h = new QHBoxLayout();
    h->setContentsMargins(0,0,0,0);
    h->setSpacing(0);
    this->setLayout(h);

#if !defined (Q_WS_MAEMO_5)  && !defined (ANDROID)  && !defined (Q_OS_IOS)
    bg1 = new BoostBarGraphWidget (this);
//    bg2 = new LambdaBarGraphWidget (this);
    bg2 = NULL;
    bg3 = new RPMBarGraphWidget (this, tr("RPM"));
    h->addWidget(bg1, 1);
//    h->addWidget(bg2, 1);
    h->addWidget(bg3, 1);
#else
    bg1=NULL;
    bg2=NULL;
    bg3=NULL;
#endif

    //ECU col
    //each ecu has a base frame with a v vox layout to which the ecu widget gets added
    fDfWidget = new QFrame (this);
    fDfWidget->setContentsMargins(0,0,0,0);
    QVBoxLayout *v2 = new QVBoxLayout();
    //disable margin around the vbox layout
    v2->setContentsMargins(0,0,0,0);
    v2->setSpacing(0);
    fDfWidget->setLayout(v2);
    h->addWidget(fDfWidget, 3);
    //Digifant 1 widget
    dfexW = new DFExtendedWidget (this, tr("Digifant I"));
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
    vr6W = new VR6Widget (this, tr("VR6 M3.8.1"));
    v4->addWidget(vr6W);

    //2. col
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
    boostW = new MeasurementWidget ( this, QString(tr("AFM (Volts)")), 0.01, 2.5, 4.5, Qt::green, Qt::green, Qt::red);
    boostW->setDigits(4);
    boostW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(boostW);

    lambdaW = new MeasurementWidget ( this, QString(tr("WbLambda")), 0.90, 1.0, 1.10, Qt::red, Qt::green, Qt::red);
    lambdaW->setDigits(4);
    lambdaW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(lambdaW);

    egtW = new MaxEgtWidget ( this, QString(tr("RPM")) );
    egtW->setDigits(4);
    egtW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(egtW);
#else

    boostW = new MeasurementWidget ( this, QString(tr("Boost [bar]")), -1, 0, 2.0, Qt::darkGreen, Qt::green, Qt::red);
    boostW->setDigits(4);
    boostW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(boostW);

    //lambdaW = new MeasurementWidget ( this, QString(tr("WB Lambda")), 0.85, 1.1, 1.36, Qt::green, Qt::yellow, Qt::red);
    //extended widget: can be switched between NB and WB
    lambdaW = new LambdaExtWidget ( this );
    connect (lambdaW, SIGNAL(showStatusMessage(QString)), this, SLOT(triggerEmitShowStatusMessage(QString)));
    lambdaW->setDigits(3.8);
    lambdaW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(lambdaW);

    egtW = new MaxEgtWidget ( this, QString("EGT") );
    egtW->setDigits(4);
    egtW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(egtW);
#endif
    bexW = new BoostExtendedWidget ( this, QString(tr("N75")) );
    bexW->setDigits(12);
    bexW->setGeometry( QRect(100,100,100,100) );
    v->addWidget(bexW);

    //3. col (md app)
    QFrame *fWidgets2 = new QFrame (this);
    fWidgets2->setContentsMargins(0,0,0,0);
    QVBoxLayout *v3 = new QVBoxLayout();
    //disable margin around the vbox layout
    v3->setContentsMargins(0,0,0,0);
    v3->setSpacing(0);
    fWidgets2->setLayout(v3);
    h->addWidget(fWidgets2, 1);
    efrW = new EFRWidget ( this, QString(tr("EFR")) );
    v3->addWidget(efrW);
    oilW = new PressureWidget (this, tr("Oil Pres"), 0.2, 0.5, 1, Qt::red, Qt::yellow, Qt::green);
    v3->addWidget(oilW);
    fuelW = new FuelPressureWidget (this, tr("Fuel Pres"));
    v3->addWidget(fuelW);
    rpmW = new MeasurementWidget (this, tr("RPM"), 0,7200,7500, QColor(Qt::green), QColor(Qt::green), QColor(Qt::red));
    if ( mdMode() )
        v3->addWidget(rpmW);
    else {
        v->addWidget(rpmW);
        fWidgets2->hide();
    }

    switchEcu();
#endif
    timer.start();

    testOverlay = new Overlay (this);

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

void RealTimeVis::showStatusMessageAsOverlay(const QString &msg)
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

            if ( bg1 ) {
                bg1->setValue( d->getSensorR()->getBoost() );
            }
            if ( bg2 ) {
                LambdaBarGraphWidget* l = qobject_cast<LambdaBarGraphWidget*>(bg2);
                if ( d->getSensorR()->getThrottle() >= 90 && l )
                    l->wotOn();
                else
                    l->wotOff();
                bg2->setValue( d->getSensorR()->getLambda() );
            }

            if ( bg3 ) {
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
            LambdaExtWidget* l = qobject_cast<LambdaExtWidget*>(lambdaW);
            if ( l != nullptr )
                l->setValue(d);
            else
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
    //qDebug() << "RealTimeVis::paintEvent" << endl;
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
#if not defined (REALTIMEVIS2021)
        fDfWidget->setVisible(true);
        fVr6Widget->setVisible(false);
#endif
        boostW->setVisible(true);
        rpmW->setVisible(true);
    }
    if ( ecuStr == "VR6" ) {
#if not defined (REALTIMEVIS2021)
        fDfWidget->setVisible(false);
        fVr6Widget->setVisible(true);
#endif
        boostW->setVisible(true);
        rpmW->setVisible(true);
    }
    if ( ecuStr == "generic (WB Lambda only)" ) {
#if not defined (REALTIMEVIS2021)
        fDfWidget->setVisible(false);
        fVr6Widget->setVisible(false);
#endif
        boostW->setVisible(false);
        rpmW->setVisible(false);
        lambdaW->setVisible(true);
    }

}

bool RealTimeVis::isDigifantApp() {
#if defined (DIGIFANTVANAPP)
    return true;
#else
    QSettings settings;
    return ! settings.value("md/md", QVariant (true)).toBool();
#endif
}
bool RealTimeVis::isMultidisplayApp() {
    QSettings settings;
    return settings.value("md/md", QVariant (true)).toBool();
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
