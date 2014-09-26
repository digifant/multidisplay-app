#include "ColorOverBlend.h"

#include <QColor>

ColorOverBlend::ColorOverBlend ()
    : lo(QColor(Qt::white)), mid(QColor(Qt::white)), hi(QColor(Qt::white)), loVal(0), midVal(0), hiVal(0)
{

}


ColorOverBlend::ColorOverBlend (QColor lo, QColor mid, QColor hi, double loVal, double midVal, double hiVal)
    : lo(lo), mid(mid), hi(hi), loVal(loVal), midVal(midVal), hiVal(hiVal)

{

}

ColorOverBlend::~ColorOverBlend () {

}

QColor ColorOverBlend::overblend2 (QColor &startColor, QColor &stopColor, short level)  {
    short redDelta = (startColor.red()*(255-level)+stopColor.red()*level)/255;
    short greenDelta = (startColor.green()*(255-level)+stopColor.green()*level)/255;
    short blueDelta = (startColor.blue()*(255-level)+stopColor.blue()*level)/255;

    QColor bQclr = QColor(redDelta,greenDelta,blueDelta);
    return bQclr;
}

QColor ColorOverBlend::overblend3 (double value) {
    if ( value <= loVal ) {
        return lo;
    }
    if ( value > loVal && value < midVal ) {
        double t = value - loVal;
        double w = midVal - loVal;
        short level = (255*t)/w;
        return overblend2 (lo, mid, (short) level);
    }
    if ( value == midVal )
        return mid;
    if ( value > midVal && value < hiVal ) {
        double t = value - midVal;
        double w = hiVal - midVal;
        short level = (255*t)/w;
        return overblend2 (mid, hi, (short) level);
    }
    if ( value >= hiVal ) {
        return hi;
    }
    return QColor(Qt::white);
}

QColor ColorOverBlend::level (double value) {
    if ( value <= loVal ) {
        return lo;
    }
    if ( value > loVal && value < midVal ) {
        return lo;
    }
    if ( value == midVal )
        return mid;
    if ( value > midVal && value < hiVal ) {
        return mid;
    }
    if ( value >= hiVal ) {
        return hi;
    }
    return QColor(Qt::white);
}
