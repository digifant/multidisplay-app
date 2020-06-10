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
    bool tapAndHoldTriggered(QTapAndHoldGesture *pTapHold);
    bool swipeTriggered(QSwipeGesture *pSwipe);
    bool gestureEvent(QGestureEvent *event);
    void paintEvent(QPaintEvent *event) override;
    void paint() override;
    void resizeEvent ( QResizeEvent * event ) override;

    bool setPositionForCol (uint lineSpacing, uint col=2 );

    quint8 df_wot_flag;

    double df_iat;
    double df_ect;
    double df_ignition;
    double df_ignition_retard;

    double df_O2_Volts; //D Starr added for O2 volts from NB lambda sensor
    double O2_Volts_max; // D Starr added for O2 volts max from NB Lambda sensor
    double df_O2_AD_Volts; //D Starr added for O2 volts at a/d

#if defined (DIGIFANTVANAPP)
    double lambda_raw;  //D Starr added for wideband
    double lambda_volts; //D Starr added for wideband volts

    quint8 df_flg3;
    quint8 df_PortC;
    quint8 df_PortD;
    quint8 df_ect_injection_addon;
    quint8 df_fu_trans_enrich;
    quint8 df_fu_trans_enrich_tmr;
    qint8 df_OXS_prop;
    qint8 df_OXS_int;
    quint8 df_WOT_MSB;
    quint8 df_WOT_LSB;
#endif
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

    bool landscape_for_text = false;
};




#endif // DFEXTENDEDWIDGET_H
