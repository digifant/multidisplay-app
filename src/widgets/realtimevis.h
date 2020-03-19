#ifndef REALTIMEVIS_H
#define REALTIMEVIS_H

#include <QGroupBox>
#include <QTime>
#include <QGridLayout>
#include "rtwidget.h"
#include <qwt_thermo.h>
#include <widgets/Overlay.h>

class BarGraphWidget;
class MdDataRecord;
class DFExtendedWidget;
class VR6Widget;

/**
 * @brief The RealTimeVis class implements the widget which visualizes all measurement values
 */
class RealTimeVis : public QWidget
{
    Q_OBJECT
public:
    explicit RealTimeVis(QWidget *parent = 0);

signals:

public slots:
    void visualize (MdDataRecord *d);
    void possibleCfgChange ();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual bool event(QEvent * e);
    void resizeEvent ( QResizeEvent * event );

    void switchEcu ();

    bool mdMode();
private:
    QwtThermo *boostT;
    BarGraphWidget *bg1;
    BarGraphWidget *bg2;
    BarGraphWidget *bg3;

    MeasurementWidget *boostW;
    MeasurementWidget *lambdaW;
    MaxEgtWidget *egtW;
    BoostExtendedWidget *bexW;
    DFWidget *dfW;
    DFExtendedWidget *dfexW;
    VR6Widget *vr6W;
    EFRWidget *efrW;
    PressureWidget *oilW;
    FuelPressureWidget *fuelW;
    MeasurementWidget *rpmW;

    Overlay* overlay;

    QFrame *fDfWidget;
    QFrame *fVr6Widget;

    QTime t;
};

#endif // REALTIMEVIS_H
