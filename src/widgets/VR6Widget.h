#ifndef VR6WIDGET_H
#define VR6WIDGET_H

#include <widgets/rtwidget.h>

class VR6Widget : public MeasurementWidget
{
public:
    VR6Widget ( QWidget *parent, QString caption="VR6", double lo=0, double mid=1, double hi=12,
                QColor loColor=(Qt::green), QColor midColor=Qt::yellow, QColor hiColor=Qt::red );
protected:
    void paint();

    uint h,w;
};

#endif // VR6WIDGET_H
