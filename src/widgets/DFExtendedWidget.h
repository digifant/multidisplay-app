#ifndef DFEXTENDEDWIDGET_H
#define DFEXTENDEDWIDGET_H

#include "rtwidget.h"
#include "MdData.h"
#include "Map16x1.h"
#include "ColorOverBlend.h"


class DFExtendedWidget : public MeasurementWidget
{
public:
    DFExtendedWidget ( QWidget *parent, QString caption, double lo=0, double mid=1, double hi=12,
            QColor loColor=(Qt::green), QColor midColor=Qt::yellow, QColor hiColor=Qt::red );

    ~DFExtendedWidget();

    void setValue( MdDataRecord *d );

protected:
    bool event(QEvent *event);
    bool swipeTriggered(QSwipeGesture *pSwipe);
    bool gestureEvent(QGestureEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void paint();
    virtual void resizeEvent ( QResizeEvent * event );

    bool setPositionForCol (uint lineSpacing, uint col=2 );

    quint8 df_wot_flag;

    double df_iat;
    double df_ect;
    double df_ignition;
    double df_ignition_retard;
    quint16 df_inj_time;
    quint8 df_voltage;
    quint8 df_boost_raw;
    quint8 df_lambda_raw;
    quint8 df_iat_enrich;
    quint8 df_ect_enrich;
    quint8 df_cold_startup_enrich;
    quint8 df_warm_startup_enrich;
    quint8 df_isv;
    quint8 df_lc_flags;
    int rpm;

    int df_knock_raw;

    double df_inj_duty;
    double injduty_max;

    Map16x1_ISV *isvMap;
    Map16x1_Voltage *voltageMap;

    QFont captionFont;

    double maxRetard;

    ColorOverBlend *rawKnockBlend;

    uint h,w;

    int m_gestureId;
};




#endif // DFEXTENDEDWIDGET_H
