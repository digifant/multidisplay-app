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
    void showStatusMessage ( const QString& );

public slots:
    void visualize (MdDataRecord *d);
    void possibleCfgChange ();

    void triggerEmitShowStatusMessage ( const QString & msg ) { emit showStatusMessage(msg); };
    void showMessage (const QString &msg, const int forSeconds=0);
    void showStatusMessageAsOverlay ( const QString &msg );
    void showMessage3 (const QString &msg);
    void hideMessage ();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual bool event(QEvent * e);
    void resizeEvent ( QResizeEvent * event );

    void switchEcu ();

    bool isDigifantApp();
    bool isMultidisplayApp();
    bool mdMode();
private:
    QGridLayout *gl = nullptr;
    QwtThermo *boostT;
    BarGraphWidget *bg1 = nullptr;
    BarGraphWidget *bg2 = nullptr;
    BarGraphWidget *bg3 = nullptr;

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

    QPointer<Overlay> testOverlay = nullptr;
    QPointer<MessageOverlay> topOverlay = nullptr;
    QPointer<MessageOverlay> middleOverlay = nullptr;
    QPointer<MessageOverlay> bottomOverlay = nullptr;

    QFrame *fDfWidget;
    QFrame *fVr6Widget;

    QElapsedTimer timer;
};

#endif // REALTIMEVIS_H
