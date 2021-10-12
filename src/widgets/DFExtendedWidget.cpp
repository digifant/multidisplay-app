#include <QPainter>
#include <QDebug>

#include "DFExtendedWidget.h"
#include "AppEngine.h"
#include <mobile/SwipeGestureRecognizer.h>

DFExtendedWidget::DFExtendedWidget ( QWidget *parent, QString caption, double lo, double mid, double hi,
                QColor loColor, QColor midColor, QColor hiColor )
    : MeasurementWidget (parent, caption, lo, mid, hi, loColor, midColor, hiColor) {

//#ifndef Q_OS_ANDROID
    captionFont.setPointSize(12);
//#else
//    captionFont.setPointSize(12);
//    textFont.setPointSize(8);
//    dataFont.setPointSize(8);
//#endif
    isvMap = new Map16x1_ISV();
    voltageMap = new Map16x1_Voltage();
    nbLambdaMap = new Map16x1_NbLambda();


    df_wot_flag = 0;

    df_iat = 0;
    df_ect = 0;
    df_ignition = 0;
    df_ignition_retard = 0;
    df_inj_time = 0;
    df_voltage = 0;
    df_boost_raw = 0;
    df_lambda_raw = 0;
    df_O2_Volts = 0;
    O2_Volts_max = 0;
    df_O2_AD_Volts = 0;
#if defined (DIGIFANTVANAPP)
    lambda_raw = 0;
    lambda_volts = 0;
    df_iat_enrich = 0;
    df_ect_enrich = 0;

    df_flg3 = 0;
    df_PortC = 0;
    df_PortD = 0;
    df_ect_injection_addon = 0;
    df_fu_trans_enrich = 0;
    df_fu_trans_enrich_tmr = 0;
    df_OXS_prop = 0;
    df_OXS_int = 0;
    df_WOT_MSB = 0;
    df_WOT_LSB = 0;
 #else
    df_iat_enrich = 0;
    df_ect_enrich = 0;
#endif
    df_cold_startup_enrich = 0;
    df_warm_startup_enrich = 0;
    df_isv = 0;
    df_lc_flags = 0;
    rpm = 0;
    df_knock_raw = 0;
    df_inj_duty = 0;

    df_ignition_retard = 0;
    maxRetard = 0;
    injduty_max = 0;

#if defined (DIGIFANTVANAPP)
    rawKnockBlend = new ColorOverBlend (QColor( Qt::red), QColor(Qt::green), QColor(Qt::red), 100, 500, 900); //use knock widget for narrow band O2 sensor
#else
    rawKnockBlend = new ColorOverBlend (QColor( Qt::green), QColor(Qt::yellow), QColor(Qt::red), 0, 60, 150);
#endif
//    grabGesture(Qt::TapGesture);
    grabGesture(Qt::TapAndHoldGesture);
//    grabGesture(Qt::PanGesture);
//    grabGesture(Qt::PinchGesture);
//    grabGesture(Qt::SwipeGesture);

    // Create a SWIPE recognizer because the default SWIPE recognizer
       // does not really work on Symbian device.
    QGestureRecognizer* pRecognizer = new SwipeGestureRecognizer();
    m_gestureId = QGestureRecognizer::registerRecognizer(pRecognizer);
}

bool DFExtendedWidget::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));
    return QWidget::event(event);
}

bool DFExtendedWidget::tapAndHoldTriggered(QTapAndHoldGesture *pTapHold)
{
    if ( injduty_max > 0 )
        injduty_max = 0;
    else
        injduty_max = 1;
    qDebug() << "tapAndHoldTriggered injduty_max = " << injduty_max;
    return true;
}

bool DFExtendedWidget::swipeTriggered(QSwipeGesture *pSwipe) {
    bool result = false;

    if (pSwipe->state() == Qt::GestureFinished) {
       qDebug("Swipe angle: %f", pSwipe->swipeAngle());
       switch (SwipeGestureUtil::GetHorizontalDirection(pSwipe)) {
          case QSwipeGesture::Left:
             qDebug("Swipe Left detected");
             result = true;
             break;
          case QSwipeGesture::Right:
             qDebug("Swipe Right detected");
             result = true;
             break;
          default:
             break;
       }
       switch (SwipeGestureUtil::GetVerticalDirection(pSwipe)) {
          case QSwipeGesture::Up:
             qDebug("Swipe Up detected");
             result = true;
             break;
          case QSwipeGesture::Down:
             qDebug("Swipe Down detected");
             result = true;
             break;
          default:
             break;
       }
    }
    return result;
}

bool DFExtendedWidget::gestureEvent(QGestureEvent *event)
{
    qDebug() << "gestureEvent():" << event->gestures().size();
    if (QGesture *swipe = event->gesture(Qt::SwipeGesture)) {
        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
        qDebug() << "swipe";
    } else if (QGesture *pan = event->gesture(Qt::PanGesture))
//        panTriggered(static_cast<QPanGesture *>(pan));
        qDebug() << "pan";
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
//        pinchTriggered(static_cast<QPinchGesture *>(pinch));
        qDebug() << "pinch";
    if (QGesture *tap = event->gesture(Qt::TapGesture))
//        tapTriggered(static_cast<QPinchGesture *>(tap));
        qDebug() << "tap";
    else if (QGesture *tapAndHold = event->gesture(Qt::TapAndHoldGesture)) {
        tapAndHoldTriggered(static_cast<QTapAndHoldGesture *>(tapAndHold));
        qDebug() << "tap and hold";
    }
    return true;
}

void DFExtendedWidget::setValue( MdDataRecord *d )
{
    this->df_lc_flags = 0;
    this->df_wot_flag = d->getSensorR()->df_flags ;
    this->df_iat = d->getSensorR()->df_iat;
    this->df_ect = d->getSensorR()->df_ect;
    this->df_ignition = d->getSensorR()->df_ignition;
    this->df_ignition_retard = d->getSensorR()->df_ignition_total_retard;
    this->df_inj_time = d->getSensorR()->df_inj_time;
    this->df_voltage = d->getSensorR()->df_voltage_raw;
    this->df_boost_raw = d->getSensorR()->df_boost_raw;
    this->df_lambda_raw = d->getSensorR()->df_lambda;
    this->df_iat_enrich = d->getSensorR()->df_iat_enrichment;
    this->df_ect_enrich = d->getSensorR()->df_ect_enrichment;
    this->df_cold_startup_enrich = d->getSensorR()->df_cold_startup_enrichment;
    this->df_warm_startup_enrich = d->getSensorR()->df_warm_startup_enrichment;
    this->df_isv = d->getSensorR()->df_isv;
    this->df_lc_flags = d->getSensorR()->df_lc_flags;
    this->df_inj_duty = d->getSensorR()->df_inj_duty;
    this->df_knock_raw = d->getSensorR()->df_knock_raw;

    if ( df_ignition_retard > maxRetard )
        maxRetard = df_ignition_retard;
    df_O2_AD_Volts = (df_lambda_raw*5.0/255);
    df_O2_Volts = (df_lambda_raw * (-7.0626000))+1661.300;  // info from Vanagon O2 analysis
    if( df_O2_Volts < 0)
        df_O2_Volts = 0;
    if( df_O2_Volts > O2_Volts_max)
        O2_Volts_max = df_O2_Volts;
    df_lambda_nb = nbLambdaMap->mapValue(df_O2_Volts);
#if defined (DIGIFANTVANAPP)
    this->lambda_raw = d->getSensorR()->getLmm(); // wideband input from small interface - 10 bit
    lambda_volts = ((lambda_raw * 5.0/1023));
    this->df_flg3 = d->getSensorR()->df_co_poti; // re-mapped to flag3
    this->df_PortC = d->getSensorR()->df_cold_startup_enrichment; // re-mapped to PortC
    this->df_PortD = d->getSensorR()->df_warm_startup_enrichment; // re-mapped to PortD
    this->df_ect_injection_addon = d->getSensorR()->df_ect_enrichment; // re-mapped to ect injection addon
    this->df_fu_trans_enrich = d->getSensorR()->df_acceleration_enrichment; // re-mpped
    this->df_fu_trans_enrich_tmr = d->getSensorR()->df_counter_startup_enrichment; // re-mapped
    this->df_OXS_prop = d->getSensorR()->df_iat_enrichment; // re-mapped to OSX_prop
    this->df_OXS_int = d->getSensorR()->df_ignition_addon_counter; // re-mapped to OXS_int
    this->df_WOT_MSB = d->getSensorR()->df_igniton_addon; // re-mapped to WOT timer
    this->df_WOT_LSB = d->getSensorR()->df_ect_injection_addon;  // re-mapped to WOT timer
#endif
    this->rpm = d->getSensorR()->getRpm();

    n75_duty = d->getSensorR()->getN75();
    speed = d->getSensorR()->getSpeed();
    if ( speed == 0 ) {
        if ( d->getMobileR() != nullptr ) {
            speed = d->getMobileR()->gpsGroundSpeed;
            speedGps = true;
        } else {
            speedGps = false;
        }
    } else {
        speedGps = false;
    }
    gear = d->getSensorR()->getGear();
    
    update();
}

DFExtendedWidget::~DFExtendedWidget () {
    if ( isvMap )
        delete isvMap;
    if ( voltageMap )
        delete voltageMap;
    if ( rawKnockBlend )
        delete rawKnockBlend;
}

void DFExtendedWidget::paintEvent(QPaintEvent *event) {
    paint ();
}

void DFExtendedWidget::paint() {
//    qDebug() << "DFExtendedWidget::paint()";

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //QColor bc = overblend(loColor, hiColor, value);
    QColor bc = loColor;
    if ( df_ignition_retard > hi)
        bc = hiColor;
    else if ( df_ignition_retard > mid )
        bc = midColor;

    painter.fillRect( QRect(0,0,size().width(),size().height()), bc);

    //    painter.setBackgroundMode( Qt::OpaqueMode );
    painter.setBackgroundMode( Qt::TransparentMode );
    painter.setBackground( QBrush ( bc ) );

    textFont.setItalic(true);
    textFont.setBold(true);
    painter.setFont(textFont);

    //scale font size to fit heigth
    //down
    while ( ( QFontMetrics(textFont).lineSpacing() * 13 ) > height() )
        textFont.setPointSizeF( textFont.pointSizeF() - 0.5 );
    //up
    while ( ( ( QFontMetrics(textFont).lineSpacing() * 14 ) < height() ) &&
            ( QFontMetrics(textFont).boundingRect(QString(tr("LC OFF   knock detection OFF"))).width() < width() ) )
        textFont.setPointSizeF( textFont.pointSizeF() + 0.5 );

    QFontMetrics fm = painter.fontMetrics();

    h = 0;
    w = 0;
    h = fm.height() + fm.leading();
    painter.drawText( QPoint(0,h), caption );

    textFont.setItalic(false);
    textFont.setBold(false);

#if defined (DIGIFANTAPP)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    w = fm.width(caption) + fm.maxWidth();
#else
    w = fm.horizontalAdvance(caption) + fm.maxWidth();
#endif

    int ps = textFont.pointSize();
    textFont.setPointSize(ps * 0.75);
    painter.setFont(textFont);
    if ( landscape_for_text )
        painter.drawText( QPoint(w,h), tr("programmed by digifant-onlineabstimmung.de") );
    w=0;
    textFont.setPointSize(ps);
    painter.setFont(textFont);
#endif

//    textFont.setBold(true);
    QString ign = "Ign " + QString::number(df_ignition, 'f', 2);
    QString retard = "Retard " + QString::number(df_ignition_retard, 'f', 2);

    //attention, QtCreator has to be in utf8 encoding (qt5) or in latin1 (qt4)
    //we should better use QTranslator http://qt-project.org/doc/qt-5/qtranslator.html#details
#if defined (DIGIFANTVANAPP)
    // this is my set-up for Vanagon
    QString ect = "";
    if (df_flg3 & 0x2)
        ect = "ECT OPEN on PWR UP";
    else if (df_ect>-50)
        ect = "ECT " + QString::number(df_ect, 'f', 1) + " " + QChar(0x00B0) + "C"; // 1 decimal
    else
        ect = "ECT is OPEN!"; // added for diagnostics
    QString iat = "";

    if (df_iat > -50)
        iat = "IAT " + QString::number(df_iat, 'f', 1) + " " + QChar(0x00B0) + "C"; // 1 decimal
   else
        iat = "IAT is OPEN!"; // added for diagnostics
    qreal df_inj_time_ms = df_inj_time / 1000.0;
    qreal injduty = df_inj_duty;
    if (injduty > injduty_max)
            injduty_max = injduty;
    QString injection = "Inj " + QString::number(df_inj_time_ms, 'f', 1) + "ms " + QString::number(injduty, 'f', 1) + " % (" + QString::number(injduty_max, 'f', 0) + "%)";
    QString boost = "AFM " + QString::number(df_boost_raw) + " (raw) " ;
    QString lambda = "O2 " + QString::number(df_lambda_raw) + " (raw)";
    QString lambda1 = "WB O2 " + QString::number(lambda_raw) + " (raw)";  // D Starr
    QString lambda2 = "WB O2 " + QString::number(lambda_volts, 'f', 2) + " Volts";  //D Starr - WB volts to 2 decimal places
    QString ect_enrich = "ECT Enrich " + QString::number(df_ect_injection_addon);
    QString OXS_PID = "OXS Prop " + QString::number(df_OXS_prop) ;
    QString OXS_INT = "OXS INT " + QString::number(df_OXS_int) ;

    QString trans_enrich = "Accel Enrich " + QString::number(df_fu_trans_enrich);
    QString trans_enrich_tmr = "Accel Enrich Tmr " + QString::number(df_fu_trans_enrich_tmr);

    quint16 WOT_conv = ((df_WOT_MSB)<<8) + (df_WOT_LSB); //convert to 16 bit
    QString WOTtimer = "WOT timer " + QString::number(WOT_conv);
    QString voltage = "Battery " + QString::number(voltageMap->mapValue(df_voltage), 'f', 1) + " Volts"; // 1 decimal places
    //QString voltage ="Battery " + QString::number(df_voltage);
    QString dk = "TPS ";
    if ( df_PortD & 0x4 )
        dk += "off - MID";
    else {
        dk += "on ";
    if ( df_wot_flag & 0x10 )
        dk += "- idle";
    else if ( df_wot_flag & 8 )
        dk += "- WOT Enrich";
    else if ( df_wot_flag | 0x10 )
        dk += "- WOT Enrich pending";
    else
        dk+="";
    }

#else
    QString ect = "";
    if (df_ect>-50)
        ect = "ECT " + QString::number(df_ect, 'f', 1) + " " + QChar(0x00B0) + "C"; // 1 decimal
    else
        ect = "ECT is OPEN!"; // added for diagnostics
    QString iat = "";

    if (df_iat > -50)
        iat = "IAT " + QString::number(df_iat, 'f', 1) + " " + QChar(0x00B0) + "C"; // 1 decimal
   else
        iat = "IAT is OPEN!"; // added for diagnostics


    qreal df_inj_time_ms = df_inj_time / 1000.0;
    qreal injduty = df_inj_duty;
    if (injduty > injduty_max)
            injduty_max = injduty;
    QString injection = "Inj " + QString::number(df_inj_time_ms, 'f', 1) + "ms " + QString::number(injduty, 'f', 1) + " % (" + QString::number(injduty_max, 'f', 0) + "%)";

    //make 0-250kpa
#if defined ( DIGIFANTAPP )
    QString boost = tr("Boost") + " " +
            QString::number( AppEngine::getInstance()->getDfBoostTransferFunction()->map( df_boost_raw ), 'f', 2 ) + " kpa";
#else
    QString boost = "Boost " + QString::number(df_boost_raw) + " (raw) "  +
            QString::number( AppEngine::getInstance()->getDfBoostTransferFunction()->map( df_boost_raw ), 'f', 2 ) + " kpa";
#endif
    QString lambda = "NB O2";
    lambda += " " + QString::number(df_lambda_nb, 'f', 2);
    QString nbLambda = "1";
    if ( df_O2_Volts > 800 )
        nbLambda = "rich";
    if ( df_O2_Volts <= 800 && df_O2_Volts > 450 )
        nbLambda = "0.99";
    if ( df_O2_Volts < 500 && df_O2_Volts > 150 )
        nbLambda = "1.01";
    if ( df_O2_Volts <= 150  )
        nbLambda = "lean";
    lambda += " " + nbLambda;

    QString ect_enrich = "ECT enrich " + QString::number(df_ect_enrich);
    QString iat_enrich = "IAT enrich " + QString::number(df_iat_enrich);

    QString cold_startup_enrich = "cold s-en " + QString::number(df_cold_startup_enrich);
    QString warm_startup_enrich = "warm s-en " + QString::number(df_warm_startup_enrich);

    quint16 isv_conv = ( 0x1a93 - ( (( isvMap->mapValue(df_isv) * 0xC7 ) / 16 ) + 0xA60 ) ) / 2;
    double isv_percent = (1 - (isv_conv / 2073.0)) * 100;
    //QString isv = "ISV " + QString::number(isv_conv) + " us " + QString::number(isv_percent) + "%";
    QString isv = "ISV open " + QString::number(isv_percent, 'f', 0) + "%";
    QString voltage = "Bat " + QString::number(voltageMap->mapValue(df_voltage), 'f', 2) + " V";
    QString lc = "LC ";
    if ( df_lc_flags & 8)
        lc += "ON";
    else
        lc += "OFF";

    QString lc_state = "";
    if ( (df_lc_flags & 3) == 3)
        lc_state = "wait for WOT-Shift";
    else {
        if ( df_lc_flags & 1)
            lc_state = "LaunchControl active";
        else if ( df_lc_flags & 4)
                lc_state = "WOT-Shift active";
    }

    QString dk = "";
    if ( df_wot_flag & 8 )
        dk = "WOT";
    else if ( df_wot_flag & 0x10 )
        dk = "idle";
    
    QString speedStr = QString::number(speed, 'f', 1) + " km/h ";
    if ( speedGps )
        speedStr += "(GPS) ";
    if ( gear > 0 )
        speedStr += " @ " + QString::number(gear) + " g";
    
    QString n75Str = "N75 " + QString::number(n75_duty/255) + " %";
#endif

    painter.setFont(textFont);
    fm = painter.fontMetrics();

    h += fm.lineSpacing();
    painter.drawText( QPoint (0, h), ign );

#if defined (DIGIFANTVANAPP)
    setPositionForCol(fm.lineSpacing(),2);
    painter.drawText( QPoint(w, h), voltage );
#else
    setPositionForCol(fm.lineSpacing(),2);
    painter.drawText( QPoint (w, h), retard + " (" + QString::number(maxRetard, 'g',2) + ")" );
#endif


    h += fm.lineSpacing();
    painter.drawText( QPoint(0, h), ect );
    setPositionForCol(fm.lineSpacing(),2);
    painter.drawText( QPoint(w,h) ,iat );

    h += fm.lineSpacing();
    painter.drawText( QPoint(0, h), injection );



//Good to here
#if defined (DIGIFANTVANAPP)
    h += fm.lineSpacing();
    painter.drawText( QPoint(0, h), boost,'f', 2 ); // AFM volts

    setPositionForCol(fm.lineSpacing(),2);

    painter.drawText( QPoint(w, h), lambda1,'f', 2 ); //Added WB lambda to second column
    h += fm.lineSpacing();

    painter.drawText( QPoint(0, h), lambda ); //narrow band O2
    setPositionForCol(fm.lineSpacing(),2);

    painter.drawText(QPoint(w, h), lambda2  ); //WB lambda volts

//add OXS PID and OXS Int here
    h += fm.lineSpacing();  //add space
    painter.drawText( QPoint(0, h), OXS_PID );
    setPositionForCol(fm.lineSpacing(),2); // 2nd column
    painter.drawText( QPoint(w, h), OXS_INT );

    h += fm.lineSpacing();  //add space
    painter.drawText( QPoint(0, h), trans_enrich ); //transient enrichment
    setPositionForCol(fm.lineSpacing(),2); // 2nd column
    painter.drawText( QPoint(w, h), trans_enrich_tmr ); //transient enrichment timer
    h += fm.lineSpacing();
    painter.drawText( QPoint(0, h), ect_enrich );
    setPositionForCol(fm.lineSpacing(),2); // 2nd column
    painter.drawText( QPoint(w, h), WOTtimer );
    h += fm.lineSpacing();
    painter.drawText( QPoint(0, h), dk ); // Throttle switch and Idle/Mid/WOT conditions

    if ( !landscape_for_text ) {
        h += 2 * fm.lineSpacing();
        QFont backupFont = textFont;
        int ps = textFont.pointSize();
        textFont.setPointSize(ps * 0.75);
        painter.setFont(textFont);
        painter.drawText( QPoint(0, h), "by digifant-onlineabstimmung.de" );
        textFont = backupFont;
        painter.setFont(textFont);
    }
#else

    h += fm.lineSpacing();
    painter.drawText( QPoint(0, h), lambda );

    h += fm.lineSpacing();
    painter.drawText( QPoint(0, h), boost );

#ifdef DIGIFANT_COMPLETE
    h += fm.lineSpacing();
    painter.drawText( QPoint(0, h), ect_enrich );
    setPositionForCol(fm.lineSpacing(),2);
    painter.drawText( QPoint(w, h), iat_enrich );

    h += fm.lineSpacing();
    painter.drawText( QPoint(0, h), cold_startup_enrich );
    setPositionForCol(fm.lineSpacing(),2);
    painter.drawText( QPoint(w, h), warm_startup_enrich );
#endif

    h += fm.lineSpacing();
    painter.drawText( QPoint(0, h), isv );
    setPositionForCol(fm.lineSpacing(),2);
    painter.drawText( QPoint(w, h), voltage );

    h += fm.lineSpacing();
    painter.drawText( QPoint(0, h), dk );


    h += fm.lineSpacing();
    painter.drawText( QPoint(0, h), lc );

    QString racemode = tr("knock detection ON");
    if (!landscape_for_text)
        racemode = tr("knock det. ON");

    if ( setPositionForCol(fm.lineSpacing(),2) )
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        w = fm.width(lc) + fm.maxWidth();
#else
        w = fm.horizontalAdvance(lc) + fm.maxWidth();
#endif
    if ( df_lc_flags & 16) {
        racemode = tr("knock detection OFF");
        if (!landscape_for_text)
            racemode = tr("knock det. OFF");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        painter.fillRect( QRect(w, h - fm.lineSpacing(),
                                fm.width (racemode), fm.height() ), Qt::red);
#else
        painter.fillRect( QRect(w, h - fm.lineSpacing(),
                                fm.horizontalAdvance(racemode), fm.height() ), Qt::red);
#endif
    }
    painter.drawText( QPoint(w, h), racemode );

    h += fm.lineSpacing();
    painter.drawText( QPoint(0, h), lc_state);

#if defined (DIGIFANTAPP)
    h += fm.lineSpacing();
    painter.drawText( QPoint(0, h), speedStr);
    if ( landscape_for_text ) {
        setPositionForCol(fm.lineSpacing(),2); // 2. column
        if ( n75_duty > 0 ) {
            painter.drawText( QPoint(w, h), n75Str);
        }
    } else {
        if ( n75_duty > 0 ) {
            h += fm.lineSpacing();
            painter.drawText( QPoint(0, h), n75Str);
        }
    }
#endif
    
    if ( !landscape_for_text ) {
        h += 2 * fm.lineSpacing();
        QFont backupFont = textFont;
        int ps = textFont.pointSize();
        textFont.setPointSize(ps * 0.75);
        painter.setFont(textFont);
        painter.drawText( QPoint(0, h), tr("by digifant-onlineabstimmung.de") );
        textFont = backupFont;
        painter.setFont(textFont);
    }
#endif

    //KNOCK Bar Test
#if defined (Q_OS_ANDROID)
    const int knockBarHeigth = QFontMetrics(textFont).lineSpacing() + QFontMetrics(textFont).leading() ;
#else
    const int knockBarHeigth = 40;
#endif
#if defined (DIGIFANTVANAPP)
    int barW = size().width() * (df_O2_Volts/1286); //1286
    painter.fillRect( QRect(0,size().height()-knockBarHeigth, barW, size().height()), rawKnockBlend->overblend3(df_O2_Volts) );
    painter.drawText( QRect(0, size().height()-knockBarHeigth + QFontMetrics(textFont).leading(), this->size().width(), this->size().height() ),
                      Qt::AlignLeft, "Nb O2 " + QString::number  ((df_O2_Volts),'f',0)+ " mV (Max " + QString::number(O2_Volts_max, 'f', 0)+ " mV)");
#else
    painter.fillRect( QRect(0,size().height()-knockBarHeigth, size().width(),size().height()), Qt::white );
    int barW = size().width() * (df_knock_raw/255.0);
    painter.fillRect( QRect(0,size().height()-knockBarHeigth, barW, size().height()), rawKnockBlend->overblend3(df_knock_raw) );

//    int textpos = knockBarHeigth - textFont.pointSize();
//    if ( textpos > 0 ) {
        painter.drawText( QRect(0, size().height()-knockBarHeigth + QFontMetrics(textFont).leading(), this->size().width(), this->size().height() ),
                          Qt::AlignLeft, tr("knock") + " " + QString::number (df_knock_raw));
//    }
#endif
    painter.end();
}

void DFExtendedWidget::resizeEvent ( QResizeEvent * event ) {
    MeasurementWidget::resizeEvent( event );
    if ( event ) {

        if ( event->size().height() > event->size().width()  )
            landscape_for_text = false;
        else
            landscape_for_text = true;
    }
}

bool DFExtendedWidget::setPositionForCol(uint lineSpacing, uint col)
{
#if defined (Q_OS_ANDROID)
    if ( landscape ) {
        w = this->size().width()/col;
    } else {
        w = 0;
        h += lineSpacing;
    }
#else
    w = this->size().width()/col;
#endif
    return landscape;
}
