#ifndef COLOROVERBLEND_H
#define COLOROVERBLEND_H

#include <QObject>
#include <QColor>

class ColorOverBlend
{
public:
    ColorOverBlend ();
    ColorOverBlend (QColor lo, QColor mid, QColor hi, double loVal, double midVal, double hiVal);
    virtual ~ColorOverBlend ();

    QColor overblend3 (double value);
    //! whithout overblending
    QColor level (double value);

signals:

public slots:

private:
    QColor overblend2 (QColor &startColor, QColor &stopColor, short level);


    QColor lo;
    QColor mid;
    QColor hi;
    double loVal;
    double midVal;
    double hiVal;
};


#endif // COLOROVERBLEND_H
