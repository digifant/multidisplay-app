#include "rtwidget.h"
#include "realtimevis.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QPainter>
#include <QWidget>
#include <QResizeEvent>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QGestureEvent>
#endif
#include <qwt_thermo.h>

#include <math.h>

#include "MdData.h"

BarGraphWidget::BarGraphWidget ( const QString & title, QWidget *parent )
    : QGroupBox (title, parent) {

    QVBoxLayout *v = new QVBoxLayout();
    setLayout(v);

    thermo = new QwtThermo (this);
    thermo->setScale(-1.2, 2.0);
    thermo->setFillBrush(  QBrush(Qt::green) );
    thermo->setAlarmBrush ( QBrush(Qt::red) );
    thermo->setAlarmLevel(1.5);
    thermo->setAlarmEnabled(true);
    thermo->setValue(0);

    v->addWidget(thermo);
}

void BarGraphWidget::resizeEvent ( QResizeEvent * event ) {
    if ( event ) {

        //HiDPI fix: size for 4-digit scale isnt sufficient which causes a loop of resize events which made the Bargraph Widget occupy the whole screen size
        if ( ((QGuiApplication*)QCoreApplication::instance())->primaryScreen()->physicalDotsPerInch() > 150 ) {
            //thermo->setScalePosition ( QwtThermo::ScalePosition::NoScale );
            thermo->setPipeWidth( event->size().width() - 150 );
        } else {
            thermo->setPipeWidth( event->size().width() - 100 );
        }
        //qDebug() << "DPI HI: " << ((QGuiApplication*)QCoreApplication::instance())->primaryScreen()->physicalDotsPerInch();
    }
}

void BarGraphWidget::setValue ( const double & value ) {
    thermo->setValue(value);
}


BoostBarGraphWidget::BoostBarGraphWidget ( QWidget * parent, const QString & title, const double & min,const double & max, const double & alarm )
    : BarGraphWidget(title, parent)
{
    //qwt5
//    thermo->setRange(min,max);
    thermo->setScale(min,max);
    thermo->setAlarmLevel(alarm);
}


RPMBarGraphWidget::RPMBarGraphWidget ( QWidget * parent, const QString & title, const double & min,const double & max, const double & alarm )
    : BarGraphWidget(title, parent)
{
    //qwt5
    //thermo->setRange(min,max);
    thermo->setScale(min,max);
    thermo->setAlarmLevel(alarm);
}


LambdaBarGraphWidget::LambdaBarGraphWidget ( QWidget * parent, const QString & title, const double & min,const double & max, const double & alarm, const double & alarmWot )
    : BarGraphWidget(title, parent), alarm(alarm), alarmWot(alarmWot), wot(false)
{
    //qwt5
//    thermo->setRange(min,max);
    thermo->setScale(min,max);
    thermo->setAlarmLevel(alarm);
}
void LambdaBarGraphWidget::wotOff() {
    if (wot) {
        thermo->setAlarmLevel(alarm);
    }
}
void LambdaBarGraphWidget::wotOn() {
    if (!wot) {
        thermo->setAlarmLevel(alarmWot);
    }
}


/* ************************************ */

#if !defined (Q_WS_MAEMO_5) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
GLGauge::GLGauge ( QWidget *parent )
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    : QGLWidget (parent) {
#else
    : QWidget (parent) {
#endif
//    : QGLWidget (parent) {
//    : QWidget (parent) {

#endif
#if defined (Q_OS_IOS)
GLGauge::GLGauge ( QWidget *parent )
    : QGLWidget (parent) {
#endif
#if defined (Q_OS_ANDROID)
GLGauge::GLGauge ( QWidget *parent )
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    : QGLWidget (parent) {
#else
    //2022-06 Qt-6.3 QGLWidget still black
    //class GLGauge : public QOpenGLWidget {
    : QWidget (parent) {
#endif
#endif

#if defined Q_OS_ANDROID
    // we use black holo theme on android (white text color) and have to set custom text color to view black text for the widgets!
    qDebug() << "GLGauge styleSheet = " << this->styleSheet();
    this->setStyleSheet( "color:black" );
    qDebug() << "GLGauge styleSheet after setting black text color = " << this->styleSheet();
#endif
}


QColor GLGauge::overblend (QColor startColor, QColor stopColor, short level) const {
    short redDelta = (startColor.red()*(255-level)+stopColor.red()*level)/255;
    short greenDelta = (startColor.green()*(255-level)+stopColor.green()*level)/255;
    short blueDelta = (startColor.blue()*(255-level)+stopColor.blue()*level)/255;

    QColor bQclr = QColor(redDelta,greenDelta,blueDelta);
    return bQclr;
}


void GLGauge::paintEvent(QPaintEvent *event) {
    //just some testing stuff...

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor b0 = overblend(Qt::green, Qt::red, 0);
    QColor b1 = overblend(Qt::green, Qt::red, 127);
    QColor b2 = overblend(Qt::green, Qt::red, 255);

    QBrush bb ( Qt::red );
    painter.setBrush(bb);
    painter.fillRect( QRect(0,0,size().width(),size().height()/3), b0);
    painter.fillRect( QRect(0, size().height()/3, size().width(), size().height()/3 ), b1);
    painter.fillRect( QRect(0, 2*(size().height()/3) ,size().width(), size().height()/3 ), b2);
    painter.setBackgroundMode( Qt::OpaqueMode );
    painter.setBackground( QBrush ( Qt::red ) );

    QPen textpen ( Qt::black );
    painter.setPen( textpen );
    QFont font;
    font.setPixelSize(30);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText( QRect(0,0,this->size().width(), 20), Qt::AlignCenter, "Value" );

    font.setPixelSize(50);
    font.setBold(true);
    painter.drawText( QRect(0,25,this->size().width(), 115), Qt::AlignLeft, "1100 C" );

    painter.end();
}

/* ************************************ */

MeasurementWidget::MeasurementWidget ( QWidget *parent, QString caption, double lo, double mid, double hi,
                                       QColor loColor, QColor midColor, QColor hiColor, float digits )
    : GLGauge (parent), caption (caption), lo(lo), mid (mid), hi(hi), value(0),
      loColor(loColor), midColor(midColor),hiColor(hiColor), digits(digits) {

    lowHeigth = false;
    landscape = true;
    recalcDataFontSize = true;

    textPen = QPen ( Qt::black );
#if defined ( Q_OS_ANDROID ) or defined ( Q_OS_IOS )
    textFont.setPointSize(16);
#else
    textFont.setPointSize(20);
#endif
    dataFont.setPixelSize(90);
//#else
//    textFont.setPointSize(10);
//    dataFont.setPointSize(50);
//#endif
    dataFont.setBold(true);

    overblend = new ColorOverBlend (loColor, midColor, hiColor, lo, mid, hi);

}


MeasurementWidget::~MeasurementWidget () {
    if ( overblend )
        delete overblend;
}

void MeasurementWidget::setValue (double nv) {
    if ( nv != value ) {
        value = nv;
        valTxt2Paint = QString::number(value);
        update();
    }
}

QColor MeasurementWidget::overblendBackground () {
    return overblend->overblend3(value);
}


void MeasurementWidget::paint() {    
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor bc = overblendBackground();
    painter.fillRect( QRect(0,0,size().width(),size().height()), bc);
    painter.setBackgroundMode( Qt::TransparentMode );
    painter.setBackground( QBrush ( bc ) );

    painter.setFont(textFont);

    quint16 h = QFontMetrics(textFont).leading();
    if ( !lowHeigth ) {
        h += QFontMetrics(textFont).lineSpacing();
        painter.drawText( QPoint(0,h), caption );
        //topline
        h += QFontMetrics(textFont).leading();
    }

    if ( recalcDataFontSize ) {
        uint dataFontPointSize = calcMaxFontPointSizeByGivenHeight (size().width(), size().height(), 1, digits);
        dataFont.setPointSize(dataFontPointSize);
        recalcDataFontSize = false;
    }

    painter.setFont(dataFont);

    if ( valTxt2PaintL2 == "" ) {
        int m = ( size().height() - h - QFontMetrics(dataFont).lineSpacing() ) / 2;
        if ( m>0 )
            h += m;
    } else {
        int m = ( size().height() - h - QFontMetrics(dataFont).lineSpacing() - QFontMetrics(textFont).lineSpacing() ) / 2;
        if ( m>0 )
            h += m;
    }

    if ( valTxt2Paint != "" )
        painter.drawText( QRect(0, (lowHeigth==false ? 0 : 0) + h, this->size().width(), this->size().height() ),
                          Qt::AlignLeft, valTxt2Paint );
    else {
        painter.drawText( QRect(0, (lowHeigth==false ? 0 : 0) + h, this->size().width(), this->size().height() ),
                                    Qt::AlignLeft, QString::number(value) );
    }

    h += QFontMetrics(textFont).leading() + QFontMetrics(dataFont).lineSpacing();
//    h += QFontMetrics(dataFont).lineSpacing();
    if ( valTxt2PaintL2 != "" ) {
        painter.setFont(textFont);
//        int h = (lowHeigth==false ? QFontMetrics(textFont).lineSpacing() : 0) + QFontMetrics(dataFont).lineSpacing();
        painter.drawText( QRect(0, h , this->size().width(), this->size().height()-h ),
                          Qt::AlignRight, valTxt2PaintL2 );
    }
    painter.end();
}

void MeasurementWidget::paintEvent(QPaintEvent *event) {
    paint ();
}

void MeasurementWidget::resizeEvent ( QResizeEvent * event ) {
    //qDebug() << "MeasurementWidget::resizeEvent width=" << event->size().width() << " height=" << event->size().height();
    if ( event ) {
        recalcDataFontSize = true;
        dataFont.setPointSize( (event->size().width()) / digits );

        if ( event->size().height() > 2 * event->size().width()  )
            landscape = false;
        else
            landscape = true;
    }
}

uint MeasurementWidget::calcMaxFontPixelSize ( uint width, uint height, float minFac, float maxFac, uint captionPointSize ) {
    QString ss;
    for ( quint8 i = 0 ; i < digits ; i++ )
        ss += "9";
    QFont cf = QFont (textFont);
    cf.setPointSize(10);
    QFontMetrics fm(cf);

    uint mywidth = width;

    if ( captionPointSize > 0 ) {
        QFont cf2 = QFont ();
        cf2.setPointSize(captionPointSize);
        QFontMetrics fm2 = QFontMetrics(cf2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        mywidth = mywidth - fm2.width(caption);
#else
        mywidth = mywidth - fm2.horizontalAdvance(caption);
#endif
    }


    bool found = false;
    bool tooHigh = false;

    while (!found) {
        fm = QFontMetrics(cf);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        uint w = fm.width(ss);
#else
        uint w = fm.horizontalAdvance(ss);
#endif
        uint h = fm.height();

        //qDebug() << "w=" << w << " h=" << h << " mywidht=" << mywidth << " ss=" << ss << " pointSize=" << cf.pointSize() ;
        if ( h > (0.8 * height) ) {
            if ( !tooHigh ) {
                cf.setPointSize( cf.pointSize() / 2 );
                tooHigh = true;
            } else {
                //abort condition
                cf.setPointSize( cf.pointSize() * 0.66 );
                found = true;
            }
        } else {

            if ( w > (maxFac * mywidth) )
                cf.setPointSize( cf.pointSize() / 2 );
            if ( w < (minFac * mywidth ) ) {
                cf.setPointSize( cf.pointSize() * 1.5 );
                fm = QFontMetrics(cf);
                if ( fm.height() > 0.8*height ) {
                    cf.setPointSize( cf.pointSize()*0.66 );
                    found = true;
                }
            } else
                found = true;
        }

        if ( cf.pointSize() < 6 ) {
            cf.setPointSize(6);
            found = true;
        }
    }
    //qDebug() << "MeasurementWidget::resizeEvent width=" << width << " calculated pointSize=" << cf.pointSize();
    return cf.pointSize();
}

uint MeasurementWidget::calcMaxFontPointSizeByGivenHeight( uint width, uint height, uint lines, float lineCharCount ) {
    QFont cf = QFont ();
    cf.setPointSize(textFont.pointSize());
    QFontMetrics fm = QFontMetrics(cf);
    bool found = false;
    uint counter = 0;
    while (!found) {
        uint th = fm.lineSpacing() * lines;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        if ( ( th > 0.95 * height ) || ( QFontMetrics(cf).width("9") * lineCharCount > width ) )
#else
        if ( ( th > 0.95 * height ) || ( QFontMetrics(cf).horizontalAdvance("9") * lineCharCount > width ) )
#endif
            cf.setPointSize( cf.pointSize()/2 );
        if ( th < 0.85 * height ) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            uint tlw = QFontMetrics(cf).width("9") * lineCharCount;
#else
            uint tlw = QFontMetrics(cf).horizontalAdvance("9") * lineCharCount;
#endif
            if (tlw < width * 0.85) {
                if ( ( tlw < width * 0.5 ) && ( th < height * 0.5) )
                    cf.setPointSize( cf.pointSize() * 2 );
                else
                    if ( ( tlw < width * 0.7  ) && ( th < height * 0.8) )
                        cf.setPointSize( cf.pointSize() * 1.2 );
                    else
                        cf.setPointSize( cf.pointSize() * 1.05 );
            } else {
                qDebug() << "INFO too wide! w=" << width << " h=" << height << " lines=" << lines << " charcount=" << lineCharCount << " th=" << th << " tlw=" << tlw;
                if (  tlw < width * 0.95 )
                    found = true;
            }
        } else
            found = true;

        fm = QFontMetrics(cf);
        ++counter;

        if ( counter >= 50 ) {
            qDebug() << "WARNING possible infinit loop detected!";
            break;
        }
    }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if ( fm.width("9") * lineCharCount > width )
#else
    if ( fm.horizontalAdvance("9") * lineCharCount > width )
#endif
        qDebug() << "MeasurementWidget::calcMaxFontPixelSize( uint width, uint height, uint lines, uint lineCharCount )" << " THIS SOULD not happen!";

    return cf.pointSize();
}


/* **************************************************** */

LambdaExtWidget::LambdaExtWidget ( QWidget *parent )
    : MeasurementWidget (parent, QString(tr("WB Lambda")), 0.85, 1.0, 1.36, QColor(Qt::green), QColor(Qt::yellow), QColor(Qt::red)) {
    nbLambdaMap = new Map16x1_NbLambda();
#if defined Q_OS_IOS
    setAttribute(Qt::WA_AcceptTouchEvents);
#endif
    grabGesture(Qt::TapGesture);
    grabGesture(Qt::TapAndHoldGesture);
}

bool LambdaExtWidget::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));
    return QWidget::event(event);
}
bool LambdaExtWidget::gestureEvent(QGestureEvent *event)
{
    bool handled = false;
    //qDebug() << "gestureEvent():" << event->gestures().size();
/*
    if (QGesture *swipe = event->gesture(Qt::SwipeGesture)) {
        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
        qDebug() << "swipe";
        handled = true;
    }
    if (QGesture *pan = event->gesture(Qt::PanGesture)) {
//        panTriggered(static_cast<QPanGesture *>(pan));
        qDebug() << "pan";
        handled = true;
    }
    if (QGesture *pinch = event->gesture(Qt::PinchGesture)) {
//        pinchTriggered(static_cast<QPinchGesture *>(pinch));
        qDebug() << "pinch";
        handled = true;
    }
*/
    if (QGesture *tap = event->gesture(Qt::TapGesture)) {
        tapTriggered(static_cast<QTapGesture *>(tap));
        qDebug() << "tap";
        handled = true;
    }
    if (QGesture *tapAndHold = event->gesture(Qt::TapAndHoldGesture)) {
        tapAndHoldTriggered(static_cast<QTapAndHoldGesture *>(tapAndHold));
        qDebug() << "tap and hold";
        handled = true;
    }
    if ( handled )
        event->accept();
    else
        qDebug() << "this should not happen!";
    return true;
}
bool LambdaExtWidget::tapTriggered(QTapGesture *pTap)
{
    if ( pTap->state() == Qt::GestureFinished ) {
        colorOnlyWOT = !colorOnlyWOT;
        if ( colorOnlyWOT )
            emit showStatusMessage(tr("Lambda color only on WOT"));
        else
            emit showStatusMessage(tr("Lambda color always"));
        qDebug() << "LambdaExtWidget tap finished! colorOnlyWOT" << colorOnlyWOT;
    }
    return true;
}

bool LambdaExtWidget::tapAndHoldTriggered(QTapAndHoldGesture *pTapHold)
{
    if ( pTapHold->state() == Qt::GestureFinished ) {
        if ( wideBand ) {
            wideBand = false;
            caption = "NB Lambda";
            emit showStatusMessage(tr("Narrowband Lambda! YOU SHOULD KNOW WHAT YOU DO! PLEASE USE A WIDEBAND LAMBDA!"));
        } else {
            wideBand = true;
            caption = "WB Lambda";
            emit showStatusMessage(tr("Wideband Lambda!"));
        }
        repaint(); //repaint
        qDebug() << "LambdaExtWidget tapAndHold finished wideBand = " << wideBand;
    }
    return true;
}
void LambdaExtWidget::setValue(MdDataRecord *d) {
    double nv = d->getSensorR()->getLambda();
    int df_lambda_raw = d->getSensorR()->df_lambda;
    //double df_O2_AD_Volts = (df_lambda_raw*5.0/255);
    double df_O2_mVolts = (df_lambda_raw * (-7.0626000))+1661.300;
    if( df_O2_mVolts < 0)
      df_O2_mVolts = 0;
    double df_lambda_nb = nbLambdaMap->mapValue(df_O2_mVolts);
    if ( wideBand ) {
        valTxt2Paint = QString::number(nv, 'f', 2);
#if defined ( DIGIFANT_LAMBDA_DEBUG )
        valTxt2PaintL2 = "NB " + QString::number(df_lambda_nb, 'f', 2);
#else
        valTxt2PaintL2 = "";
#endif
    } else {
        nv = df_lambda_nb;
        valTxt2Paint = QString::number(nv, 'f', 2);
        //valTxt2PaintL2 = QString(tr("narrowband!") );
        valTxt2PaintL2 = "narrowband! " + QString::number(df_O2_mVolts/1000.0, 'f', 2) + " V \nWB " + QString::number(d->getSensorR()->getLambda(), 'f', 2);
    }
    if ( nv != value ) {
        if ( d->getSensorR()->df_flags & 8 )
            load = 100;
        else
            load = 0;
        value = nv;
        update();
    } else
        //FIX 2021-06 actual value not drawn! :(
        update();
}

void LambdaExtWidget::paint() {
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor bc = QColor(Qt::white);
    if ( !colorOnlyWOT || (load >= 100) )
        bc = overblendBackground();
    painter.fillRect( QRect(0,0,size().width(),size().height()), bc);
    painter.setBackgroundMode( Qt::TransparentMode );
    painter.setBackground( QBrush ( bc ) );

    painter.setFont(textFont);

    quint16 h = QFontMetrics(textFont).leading();
    if ( !lowHeigth ) {
        h += QFontMetrics(textFont).lineSpacing();
        painter.drawText( QPoint(0,h), caption );
        //topline
        h += QFontMetrics(textFont).leading();
    }

    if ( recalcDataFontSize ) {
        uint dataFontPointSize = calcMaxFontPointSizeByGivenHeight (size().width(), size().height(), 1, digits);
        dataFont.setPointSize(dataFontPointSize);
        recalcDataFontSize = false;
    }

    painter.setFont(dataFont);

    if ( valTxt2PaintL2 == "" ) {
        int m = ( size().height() - h - QFontMetrics(dataFont).lineSpacing() ) / 2;
        if ( m>0 )
            h += m;
    } else {
        int m = ( size().height() - h - QFontMetrics(dataFont).lineSpacing() - QFontMetrics(textFont).lineSpacing() ) / 2;
        if ( m>0 )
            h += m;
    }

    if ( valTxt2Paint != "" )
        painter.drawText( QRect(0, (lowHeigth==false ? 0 : 0) + h, this->size().width(), this->size().height() ),
                          Qt::AlignLeft, valTxt2Paint );
    else {
        painter.drawText( QRect(0, (lowHeigth==false ? 0 : 0) + h, this->size().width(), this->size().height() ),
                                    Qt::AlignLeft, QString::number(value) );
    }

    h += QFontMetrics(textFont).leading() + QFontMetrics(dataFont).lineSpacing();
//    h += QFontMetrics(dataFont).lineSpacing();
    if ( valTxt2PaintL2 != "" ) {
        painter.setFont(textFont);
//        int h = (lowHeigth==false ? QFontMetrics(textFont).lineSpacing() : 0) + QFontMetrics(dataFont).lineSpacing();
        painter.drawText( QRect(0, h , this->size().width(), this->size().height()-h ),
                          Qt::AlignRight, valTxt2PaintL2 );
    }
    painter.end();
}


/* **************************************************** */

MaxEgtWidget::MaxEgtWidget ( QWidget *parent, QString caption, double lo, double mid, double hi,
                             QColor loColor, QColor midColor, QColor hiColor )
    : MeasurementWidget (parent, caption, lo, mid, hi, loColor, midColor, hiColor) {
}

void MaxEgtWidget::setValue (double value, quint8 idx) {
    if ( this->value != value || this->idx != idx ) {
        this->value = value;
        this->idx = idx;
        if ( value < 1800 )
            valTxt2Paint = QString::number(value);
        else
            valTxt2Paint = "nc";
        valTxt2PaintL2 = QString::number(idx);
        update();
    }
}

void MaxEgtWidget::paint() {
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor bc = overblend->overblend3(value);
    painter.fillRect( QRect(0,0,size().width(),size().height()), bc);
    painter.setBackgroundMode( Qt::TransparentMode );
    painter.setBackground( QBrush ( bc ) );

    painter.setFont(textFont);

    QString max_caption = caption;
    if ( valTxt2PaintL2 != "" )
        max_caption += " max " +  valTxt2PaintL2;
    uint h = QFontMetrics(textFont).lineSpacing();
//    h+= QFontMetrics(textFont).leading();
    painter.drawText( QPoint(0,h), max_caption );
    h += QFontMetrics(textFont).leading();


    if ( recalcDataFontSize ) {
        uint dataFontPointSize = calcMaxFontPointSizeByGivenHeight (size().width(), size().height(), 1, digits);
        dataFont.setPointSize(dataFontPointSize);
        recalcDataFontSize = false;
    }

    painter.setFont(dataFont);

//    int m = ( size().height() - h - QFontMetrics(dataFont).lineSpacing() ) / 2;
//    if ( m>0 )
//        h += m;

    if ( valTxt2Paint != "" )
        painter.drawText( QRect(0, (lowHeigth==false ? 0 : 0) + h, this->size().width(), this->size().height() ),
                          Qt::AlignLeft, valTxt2Paint );
    else {
        painter.drawText( QRect(0, (lowHeigth==false ? 0 : 0) + h, this->size().width(), this->size().height() ),
                                    Qt::AlignLeft, QString::number(value) );
    }

//    if ( valTxt2Paint != "" )
//        painter.drawText( QRect(0, (lowHeigth==false ? textFont.pointSize() : 0) + 10, this->size().width(), this->size().height() ),
//                          Qt::AlignLeft, valTxt2Paint );
//    else
//        painter.drawText( QRect(0, (lowHeigth==false ? textFont.pointSize() : 0) + 10, this->size().width(), this->size().height() ),
//                          Qt::AlignLeft, QString::number(value) );

    if ( valTxt2PaintL2 != "" ) {
        painter.setFont(textFont);
#ifndef Q_WS_MAEMO_5
        //        painter.drawText( QRect(0, (wideMode==false ? textFont.pointSize() : 0) + dataFont.pointSize() + 20, this->size().width(), this->size().height() ),
        //                          Qt::AlignRight, valTxt2PaintL2 );
        int h = (lowHeigth==false ? textFont.pointSize() : 0) + dataFont.pointSize() + 20;
        //no space for a third line on n900
        painter.drawText( QRect(0, h , this->size().width(), this->size().height()-h ),
                          Qt::AlignRight, valTxt2PaintL2 );
#endif
    }
    painter.end();
}



/* **************************************************** */

BoostExtendedWidget::BoostExtendedWidget ( QWidget *parent, QString caption, double lo, double mid, double hi,
                             QColor loColor, QColor midColor, QColor hiColor )
    : MeasurementWidget (parent, caption, lo, mid, hi, loColor, midColor, hiColor) {

}

void BoostExtendedWidget::setValue(MdDataRecord *dr) {
#if defined (DIGIFANTVANAPP)
    // D Starr
 //   qreal afm_volts = (dr->getSensorR()-> df_boost_raw) * 5.00 / 255;
 //   double boost = afm_volts;
      double boost = (dr->getSensorR()->df_boost_raw)*5.00/255;
#else
    double boost = dr->getSensorR()->getBoost();
#endif
    quint8 n75_duty = dr->getSensorR()->getN75();
    quint8 n75_map_duty = dr->getSensorR()->getN75ReqBoostPWM();
    double n75_map_requested_boost = dr->getSensorR()->getN75ReqBoost();
    quint16 speed = dr->getSensorR()->getSpeed();
    quint8 gear = dr->getSensorR()->getGear();

    if ( this->value != boost || this->n75_duty != n75_duty || this->n75_map_duty != n75_map_duty
         || this->n75_map_requested_boost != n75_map_requested_boost || this->speed != speed || this->gear != gear) {
        this->value = boost;
        this->n75_duty = n75_duty;
        this->n75_map_duty = n75_map_duty;
        this->n75_map_requested_boost = n75_map_requested_boost;
        this->speed = speed;
        this->gear = gear;

        //Speed / Gear
        valTxt2Paint = QString::number(speed) + " km/h @ " + QString::number(gear) + " g";
        valTxt2PaintL2 = "N75 " + QString::number(n75_duty) + " | " + QString::number(n75_map_duty)
                + " | " + QString::number(n75_map_requested_boost);
        update();
    }
}



/* **************************************************** */

DFWidget::DFWidget ( QWidget *parent, QString caption, double lo, double mid, double hi,
                QColor loColor, QColor midColor, QColor hiColor )
    : MeasurementWidget (parent, caption, lo, mid, hi, loColor, midColor, hiColor) {

    captionFont.setPointSize(12);

    update();
}

void DFWidget::setValue( quint8 df_lc_flag, quint8 df_wot_flag, double df_iat, double df_ect,
              double df_ignition, double df_ignition_retard, quint16 df_inj_time,
              quint8 df_voltage )
{
    //repaint only if necessary?
    this->df_lc_flag = df_lc_flag;
    this->df_wot_flag = df_wot_flag;
    this->df_iat = df_iat;
    this->df_ect = df_ect;
    this->df_ignition = df_ignition;
    this->df_ignition_retard = df_ignition_retard;
    this->df_inj_time = df_inj_time;
    this->df_voltage = df_voltage;

    update();
}

void DFWidget::paint() {
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //QColor bc = overblend(loColor, hiColor, value);
    QColor bc = loColor;

    painter.fillRect( QRect(0,0,size().width(),size().height()), bc);
    //    painter.setBackgroundMode( Qt::OpaqueMode );
    painter.setBackgroundMode( Qt::TransparentMode );
    painter.setBackground( QBrush ( bc ) );

    painter.setFont(textFont);
//    painter.drawText( QPoint(0,textFont.pointSize()), caption );

    QString valTxt2Paint = "LC S " + QString::number(df_lc_flag & 7) + " | " + QString::number(df_lc_flag & 8);
    QString valTxt2PaintL2 = "Ign " + QString::number(df_ignition, 'f', 2) + " | R=" + QString::number(df_ignition_retard, 'f', 2);
    QString valTxt2PaintL3 = "ECT " + QString::number(df_ect, 'f', 1) + " | IAT " + QString::number(df_iat, 'f', 1);
    QString valTxt2PaintL4 = "Inj=" + QString::number(df_inj_time);

//    painter.setFont(dataFont);
    painter.setFont(textFont);
    painter.drawText( QRect(0, textFont.pixelSize() + 20, this->size().width(), this->size().height() ),
                     Qt::AlignLeft, valTxt2Paint);
    painter.setFont(textFont);
    painter.drawText( QRect(0, textFont.pixelSize() + 40 + 5, this->size().width(), this->size().height() ),
                         Qt::AlignLeft, valTxt2PaintL2 );
    painter.drawText( QRect(0, textFont.pixelSize() + 60 + 10, this->size().width(), this->size().height() ),
                         Qt::AlignLeft, valTxt2PaintL3 );
    painter.drawText( QRect(0, textFont.pixelSize() + 80 + 15, this->size().width(), this->size().height() ),
                         Qt::AlignLeft, valTxt2PaintL4 );

    painter.end();
}

/* **************************************************** */
PressureWidget::PressureWidget ( QWidget *parent, QString caption, double lo, double mid, double hi,
                                 QColor loColor, QColor midColor, QColor hiColor )
    : MeasurementWidget (parent, caption, lo, hi, mid, loColor, midColor, hiColor)
{
}

void PressureWidget::setValue(double pressure)
{
    value = pressure;
    valTxt2Paint = QString::number( (pressure / 1000.0), 'f', 2 );
    update();
}

void PressureWidget::paint()
{
    MeasurementWidget::paint();
}
QColor PressureWidget::overblendBackground () {
    return overblend->level(value);
}

/* **************************************************** */
EFRWidget::EFRWidget(QWidget *parent, QString caption, double lo, double mid, double hi,
                     QColor loColor, QColor midColor, QColor hiColor)
    : MeasurementWidget (parent, caption, lo, mid, hi, loColor, midColor, hiColor, 5) {

}

void EFRWidget::paint()
{
    MeasurementWidget::paint();
}

void EFRWidget::setValue(double speed)
{
    value = speed;
    valTxt2Paint = QString::number( ceil (speed / 1000.0) ) + " K";
    update();
}

/* **************************************************** */
FuelPressureWidget::FuelPressureWidget(QWidget *parent, QString caption, double fuelRailBasePressure)
    : MeasurementWidget (parent, caption, 0.1, 0.2, 0.5, QColor(Qt::green), QColor(Qt::yellow), QColor(Qt::red)),
      fuelRailBasePressure(fuelRailBasePressure)
{

}

void FuelPressureWidget::setValue(double pressure, double boost)
{
    value = pressure/1000;
    this->boost = boost;
    valTxt2Paint = QString::number( (pressure / 1000.0), 'f', 2 );
    update();
}

void FuelPressureWidget::paint()
{
    MeasurementWidget::paint();
}

QColor FuelPressureWidget::overblendBackground()
{
    if ( value < 0.5 )
        return QColor(Qt::white);
    if ( boost < .5 )
        return QColor(Qt::green);

    double need = fuelRailBasePressure + boost;
    double deviation = need - value;
    //negative is great
    //positive means too low fuel pressure!
    return overblend->overblend3(deviation);
}




