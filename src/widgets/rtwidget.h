#ifndef RTWIDGET_H
#define RTWIDGET_H

#include <QGroupBox>
#include <QPen>
#include <QColor>
#include <QFrame>

#include "ColorOverBlend.h"

#include <QtOpenGL>

class QwtThermo;
class MdDataRecord;

class BarGraphWidget : public QGroupBox
{
    Q_OBJECT
public:
//    BarGraphWidget ( QWidget * parent = 0 );
    BarGraphWidget ( const QString & title, QWidget * parent = 0 );

    QwtThermo* getThermoWidget();
    void setValue ( const double & value );
//    void setPipeWidth ( int w ) { thermo->setPipeWidth(w); };
    void resizeEvent ( QResizeEvent * event );

protected:
    QwtThermo *thermo;
};

#if defined (DIGIFANTVANAPP)
    class BoostBarGraphWidget : public BarGraphWidget
    {
        Q_OBJECT
    public:
        BoostBarGraphWidget ( QWidget * parent = nullptr, const QString & title = "AFM-Volts", const double & min = 0.1, const double & max = 5.0 , const double & alarm = 4.5   );
    };
    class RPMBarGraphWidget : public BarGraphWidget
    {
        Q_OBJECT
    public:
        RPMBarGraphWidget ( QWidget * parent = nullptr, const QString & title = "RPM", const double & min = 0, const double & max = 6000 , const double & alarm = 4500   );
    };

#else
class BoostBarGraphWidget : public BarGraphWidget
{
    Q_OBJECT
public:
    BoostBarGraphWidget ( QWidget * parent = 0, const QString & title = "Boost", const double & min = -1, const double & max = 2.2 , const double & alarm = 1.5   );
};

class RPMBarGraphWidget : public BarGraphWidget
{
    Q_OBJECT
public:
    RPMBarGraphWidget ( QWidget * parent = 0, const QString & title = "RPM", const double & min = 0, const double & max = 8000 , const double & alarm = 7000   );
};
#endif

class LambdaBarGraphWidget : public BarGraphWidget
{
    Q_OBJECT
public:
    LambdaBarGraphWidget ( QWidget * parent = 0, const QString & title = "Lambda", const double & min = 0.68, const double & max = 1.36 , const double & alarm = 1.1 , const double & alarmWot = 0.85   );
    void wotOn ();
    void wotOff ();


protected:
    double alarm;
    double alarmWot;
    bool wot;
};


#if !defined (Q_WS_MAEMO_5) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
class GLGauge : public QGLWidget {
//class GLGauge : public QFrame {
#else
class GLGauge : public QFrame {
#endif

public:
    GLGauge ( QWidget *parent );

protected:
    virtual void paintEvent(QPaintEvent *event);

    QColor overblend(QColor startColor, QColor stopColor, short level) const;

};

class MeasurementWidget : public GLGauge {
    Q_OBJECT
public:
    MeasurementWidget ( QWidget *parent, QString caption, double lo=0, double mid=600, double hi=920,
                        QColor loColor=QColor(Qt::cyan), QColor midColor=QColor(Qt::green), QColor hiColor=Qt::red, float digits=4 );
    virtual ~MeasurementWidget();

    virtual void setValue (double);
    void setDigits (float d) { digits = d; };

    void setLowHeigth ( bool s ) { lowHeigth = s;}
    bool getLowHeigth () { return lowHeigth;}

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void paint();
    virtual QColor overblendBackground ();

//    QColor overblend(QColor startColor, QColor stopColor, double value) const;
    virtual void resizeEvent ( QResizeEvent * event );

    //! TODO test me
    uint calcMaxFontPixelSize ( uint width, uint height, float minFac, float maxFac, uint captionPointSize=0 );

    uint calcMaxFontPointSizeByGivenHeight (uint width, uint height, uint lines, float lineCharCount );

    QString caption;
    double lo;
    double mid;
    double hi;
//    double startBlend;
    double value;

    QColor loColor;
    QColor midColor;
    QColor hiColor;

    float digits;

    QPen textPen;
    QFont textFont;
    QFont dataFont;

    QString valTxt2Paint;
    QString valTxt2PaintL2;

    ColorOverBlend *overblend;

    //! if set mostly widget displays only 1 line
    bool lowHeigth;
    //! set if widgets width > heigth
    bool landscape;
    bool recalcDataFontSize;
};

class MaxEgtWidget : public MeasurementWidget {
    Q_OBJECT
public:
    MaxEgtWidget ( QWidget *parent, QString caption, double lo=0, double mid=800, double hi=920,
                    QColor loColor=Qt::cyan, QColor midColor=QColor(Qt::green), QColor hiColor=Qt::red );

    void setValue(double egt, quint8 idx);
    virtual void paint();

protected:
    quint8 idx;
};

class BoostExtendedWidget : public MeasurementWidget {
    Q_OBJECT
public:
    BoostExtendedWidget ( QWidget *parent, QString caption, double lo=0, double mid=0, double hi=2,
                    QColor loColor=QColor(Qt::darkGreen), QColor midColor=QColor(Qt::green), QColor hiColor=Qt::red );

    void setValue(MdDataRecord* dr);

protected:
    quint8 n75_duty;
    quint8 n75_map_duty;
    double n75_map_requested_boost;
    quint16 speed;
    quint8 gear;
};

class EfrWidget : public GLGauge {
    Q_OBJECT
public:
    EfrWidget ( QWidget* parent );
    void setValue(MdDataRecord* dr);
protected:
    double efr_speed;
    double boost;

    double boostLo;
    double boostHi;
    double efrSpeedLo;
    double efrSpeedHi;
};

class DFWidget : public MeasurementWidget {
    Q_OBJECT
public:
    DFWidget ( QWidget *parent, QString caption, double lo=0, double mid=0, double hi=980,
                    QColor loColor=QColor(Qt::green), QColor midColor=Qt::green, QColor hiColor=Qt::red );

    void setValue( quint8 df_lc_flag, quint8 df_wot_flag, double df_iat, double df_ect,
                  double df_ignition, double df_ignition_retard, quint16 df_inj_time,
                  quint8 df_voltage );

protected:
    virtual void paint();

    quint8 df_lc_flag;
    quint8 df_wot_flag;

    double df_iat;
    double df_ect;
    double df_ignition;
    double df_ignition_retard;
    quint16 df_inj_time;
    quint8 df_voltage;

    QFont captionFont;
};

class EFRWidget : public MeasurementWidget {
    Q_OBJECT
public:
    EFRWidget ( QWidget *parent, QString caption, double lo=0, double mid=50000, double hi=150000,
                    QColor loColor=QColor(Qt::cyan), QColor midColor=Qt::green, QColor hiColor=Qt::red );

    void setValue(double speed);
    virtual void paint();

protected:
    quint8 idx;
};
class PressureWidget : public MeasurementWidget {
    Q_OBJECT
public:
    PressureWidget ( QWidget *parent, QString caption, double lo=0, double mid=500, double hi=980,
                    QColor loColor=QColor(Qt::green), QColor midColor=Qt::green, QColor hiColor=Qt::red );

    void setValue(double pressure);
    virtual void paint();
    virtual QColor overblendBackground ();

protected:
    quint8 idx;
};

class FuelPressureWidget : public MeasurementWidget {
    Q_OBJECT
public:
    FuelPressureWidget ( QWidget *parent, QString caption, double fuelRailBasePressure=3 );

    void setValue(double pressure, double boost);
    virtual void paint();
    virtual QColor overblendBackground ();

protected:
    double fuelRailBasePressure;
    double boost;
};
#endif // RTWIDGET_H
