#include <QPainter>
#include <QDebug>

#include "DFExtendedWidget.h"
#include "AppEngine.h"

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
    df_ignition_retard = 0;
    maxRetard = 0;
    injduty_max = 0;

    rawKnockBlend = new ColorOverBlend (QColor( Qt::green), QColor(Qt::yellow), QColor(Qt::red), 0, 60, 150);
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

    this->rpm = d->getSensorR()->getRpm();

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
    QFontMetrics fm = painter.fontMetrics();
    uint h = 0;
//#ifndef Q_OS_ANDROID
    h = fm.height()/2 + 5;
    //painter.drawText( QPoint(0,textFont.pointSize()), caption );
    painter.drawText( QPoint(0,h), caption );
//#else
//    painter.drawText( QPoint(0,painter.fontMetrics().height()/2), caption );
//#endif
    textFont.setItalic(false);
    textFont.setBold(false);

//    textFont.setBold(true);
    QString ign = "Ign " + QString::number(df_ignition, 'f', 2);
    QString retard = "Retard " + QString::number(df_ignition_retard, 'f', 2);

    //attention, QtCreator has to be in utf8 encoding (qt5) or in latin1 (qt4)
    //we should better use QTranslator http://qt-project.org/doc/qt-5/qtranslator.html#details
    QString ect = "ECT " + QString::number(df_ect, 'f', 1) + " �C";
    QString iat = "IAT " + QString::number(df_iat, 'f', 1) + " �C";


    qreal df_inj_time_ms = df_inj_time / 1000.0;
//    qreal injduty = 2* ((df_inj_time_ms * rpm)/1200);
    qreal injduty = df_inj_duty;
    if (injduty > injduty_max)
            injduty_max = injduty;
//    if ( rpm > 0 )
//        injduty = (df_inj_time_ms *100) / (1000 / (rpm / 60));
    QString injection = "Inj " + QString::number(df_inj_time_ms, 'f', 1) + "ms " + QString::number(injduty, 'f', 1) + " % (" + QString::number(injduty_max, 'f', 0) + "%)";

    //make 0-250kpa
    QString boost = "Boost " + QString::number(df_boost_raw) + " (raw) "  +
            QString::number( AppEngine::getInstance()->getDfBoostTransferFunction()->map( df_boost_raw ) ) + " kpa";

    QString lambda = "Lambda " + QString::number(df_lambda_raw) + "(raw)";

    QString ect_enrich = "ECT enrich " + QString::number(df_ect_enrich);
    QString iat_enrich = "IAT enrich " + QString::number(df_iat_enrich);

    QString cold_startup_enrich = "cold s-en " + QString::number(df_cold_startup_enrich);
    QString warm_startup_enrich = "warm s-en " + QString::number(df_warm_startup_enrich);

    quint16 isv_conv = ( 0x1a93 - ( (( isvMap->mapValue(df_isv) * 0xC7 ) / 16 ) + 0xA60 ) ) / 2;
    QString isv = "ISV " + QString::number(isv_conv) + " usecs";
    QString voltage = "Volt " + QString::number(voltageMap->mapValue(df_voltage)) + " V";
//    QString isv = "ISV " + QString::number(df_isv) + " raw";
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

//#ifndef Q_OS_ANDROID
//    painter.setFont(dataFont);
    painter.setFont(textFont);
    fm = painter.fontMetrics();

//    painter.drawText( QRect(0, textFont.pixelSize() + 20, this->size().width(), this->size().height() ),
//                     Qt::AlignLeft, valTxt2Paint);
    h += fm.height()/2 + fm.lineSpacing();
//    painter.drawText( QRect(0, textFont.pixelSize() + 2*textFont.pointSize() + 5, this->size().width(), this->size().height() ),
//                         Qt::AlignLeft, ign );
//    painter.drawText( QRect(this->size().width()/2, textFont.pixelSize() + 2*textFont.pointSize() + 5, this->size().width()/2, this->size().height() ),
//                         Qt::AlignLeft, retard + " (" + QString::number(maxRetard, 'g',2) + ")" );
    painter.drawText( QPoint (0, h), ign );
    painter.drawText( QPoint (this->size().width()/2, h), retard + " (" + QString::number(maxRetard, 'g',2) + ")" );


    painter.drawText( QRect(0, textFont.pixelSize() + 3*textFont.pointSize() + 10, this->size().width(), this->size().height() ),
                         Qt::AlignLeft, ect );
    painter.drawText( QRect(this->size().width()/2, textFont.pixelSize() + 3*textFont.pointSize() + 10, this->size().width()/2, this->size().height() ),
                         Qt::AlignLeft, iat );

    painter.drawText( QRect(0, textFont.pixelSize() + 4*textFont.pointSize() + 15, this->size().width(), this->size().height() ),
                         Qt::AlignLeft, injection );
//    painter.drawText( QRect(this->size().width()/2, textFont.pixelSize() + 4*textFont.pointSize() + 15, this->size().width()/2, this->size().height() ),
//                         Qt::AlignLeft, iat );

    painter.drawText( QRect(0, textFont.pixelSize() + 5*textFont.pointSize() + 20, this->size().width(), this->size().height() ),
                         Qt::AlignLeft, lambda );

    painter.drawText( QRect(0, textFont.pixelSize() + 6*textFont.pointSize() + 25, this->size().width(), this->size().height() ),
                         Qt::AlignLeft, boost );

    painter.drawText( QRect(0, textFont.pixelSize() + 7*textFont.pointSize() + 30, this->size().width(), this->size().height() ),
                         Qt::AlignLeft, ect_enrich );
    painter.drawText( QRect(this->size().width()/2, textFont.pixelSize() + 7*textFont.pointSize() + 30, this->size().width()/2, this->size().height() ),
                         Qt::AlignLeft, iat_enrich );

    painter.drawText( QRect(0, textFont.pixelSize() + 8*textFont.pointSize() + 35, this->size().width(), this->size().height() ),
                         Qt::AlignLeft, cold_startup_enrich );
    painter.drawText( QRect(this->size().width()/2, textFont.pixelSize() + 8*textFont.pointSize() + 35, this->size().width()/2, this->size().height() ),
                         Qt::AlignLeft, warm_startup_enrich );

    painter.drawText( QRect(0, textFont.pixelSize() + 9*textFont.pointSize() + 40, this->size().width(), this->size().height() ),
                         Qt::AlignLeft, isv );
    painter.drawText( QRect(this->size().width()/2, textFont.pixelSize() + 9*textFont.pointSize() + 40, this->size().width()/2, this->size().height() ),
                         Qt::AlignLeft, voltage );

    painter.drawText( QRect(0, textFont.pixelSize() + 10*textFont.pointSize() + 45, this->size().width(), this->size().height() ),
                         Qt::AlignLeft, dk );


    painter.drawText( QRect(0, textFont.pixelSize() + 11*textFont.pointSize() + 70, this->size().width(), this->size().height() ),
                         Qt::AlignLeft, lc );

    QString racemode = "knock detection ON";
    if ( df_lc_flags & 16) {
        racemode = "knock detection OFF";
        painter.fillRect( QRect(fm.width(lc+2), textFont.pixelSize() + 11*textFont.pointSize() + 70,
                                fm.width (racemode), fm.height() ), Qt::red);
    }
    painter.drawText( QRect(fm.width(lc+2), textFont.pixelSize() + 11*textFont.pointSize() + 70, this->size().width(), this->size().height() ),
                         Qt::AlignLeft, racemode );

    painter.drawText( QRect(0, textFont.pixelSize() + 12*textFont.pointSize() + 75, this->size().width(), this->size().height() ),
                         Qt::AlignLeft, lc_state);
//#else
//    QFontMetrics fmt = QFontMetrics(textFont);
//    textFont.setPointSize(10);
//    painter.setFont(textFont);
//    //y pos is baseline!
//    h = fmt.height();
//    painter.drawText( QPoint(0,h), caption );
//    h += fmt.height();
////    painter.drawText( QPoint(0,h), ign);
////    painter.drawText( QPoint(this->size().width()/2, h), retard + " (" + QString::number(maxRetard, 'g',2) + ")" );
//    h += fmt.height();

//#endif

    //KNOCK Bar Test
    const int knockBarHeigth = 40;
    painter.fillRect( QRect(0,size().height()-knockBarHeigth, size().width(),size().height()), Qt::white );
    int barW = size().width() * (df_knock_raw/255.0);
    painter.fillRect( QRect(0,size().height()-knockBarHeigth, barW, size().height()), rawKnockBlend->overblend3(df_knock_raw) );

    int textpos = knockBarHeigth - textFont.pointSize();
    if ( textpos > 0 ) {
        painter.drawText( QRect(0, size().height()-knockBarHeigth + textFont.pixelSize() + textpos/2, this->size().width(), this->size().height() ),
                          Qt::AlignLeft, "knock " + QString::number (df_knock_raw));
    }

    painter.end();
}

void DFExtendedWidget::resizeEvent ( QResizeEvent * event ) {
    qDebug() << "DFExtendedWidget::resizeEvent width=" << event->size().width() << " height=" << event->size().height();
    if ( event ) {
#ifndef Q_OS_ANDROID
        //ignore it at the moment
#else
//        uint ps = calcMaxFontPixelSizeByGivenHeight ( event->size().width(), event->size().height(), 14, 30 );
//        qDebug() << "DFExtendedWidget::resizeEvent calculated pointSize=" << ps;
//        textFont.setPointSize(ps);
#endif
    }
}
