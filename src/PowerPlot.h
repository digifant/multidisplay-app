#ifndef POWERPLOT_H
#define POWERPLOT_H

#include <QObject>
#include "MdPlot.h"

class ComputedPowerData {
public:
    ComputedPowerData () : kw_wheel(0), torque_wheel(0), kw_engine(0), torque_engine(0), kw_engine_din(0), torque_engine_din(0) {};
    qreal kw_wheel;
    qreal torque_wheel;
    qreal kw_engine;
    qreal torque_engine;
    qreal kw_engine_din;
    qreal torque_engine_din;
};

class SpeedData {
public:
    SpeedData() : time_s(0), time_delta(0), alt_delta(0), alt(0) {};
    qreal time_s;
    qreal time_delta;
    qreal alt_delta;
    qreal alt;
};

class PowerPlot : public MdPlot
{
    Q_OBJECT
public:
    //explicit PowerPlot(QObject *parent = 0);
    PowerPlot ( QMainWindow* mw, QWidget* parent, QTableView *tableView=NULL );

    //!unused
    void addRecord(MdSensorRecord *r, bool doReplot) {};

    void setData ( QList<MdDataRecord*>&, QList<int>&, bool useGpsSpeed=false );

    void clear ();

    void computePower (MdDataRecord *curR, MdDataRecord *lastR, ComputedPowerData* cp, int lastRowNr=-1, int rowNr=1, bool useGpsSpeed=false);

    void setXBottomScaling ();
    void configAxis();

    qreal interpolate (qreal x1, qreal y1, qreal x2, qreal y2, qreal xs);
    qreal findX (qreal x1, qreal y1, qreal x2, qreal y2, qreal ys);
    QMap<qreal,SpeedData> calculateTimeBetweenSpeeds ( QList<MdDataRecord*>&dl, QList<int> &rn, qreal startspeed, qreal endspeed );
    QMap<qreal,SpeedData> calculateTimeBetweenSpeedsGPS ( QList<MdDataRecord*>&dl, QList<int> &rn, qreal startspeed, qreal endspeed );
    QMap<qreal,SpeedData> calculateAdditionalDataTimeBetweenSpeeds ( QList<MdDataRecord*>&dl, QList<int> &rn, qreal speedL, qreal speedH, bool useGps );

    //! returns map indexed by rownum
    QMap<int,double> smoothSpeedMovingAverage (bool useGpsSpeed);
    QMap<int,double> smoothRPMMovingAverage ();
    QMap<int,double> smoothTimeMovingAverage ();
    void smoothMovingAverage (MdPlotData* d, const quint8 smooth);
    QList<double> smoothMovingAverage (const QList<double> &data, const quint8 smooth);

signals:
    void resultString (QString);

public slots:
    qreal dinTemp() {return din_temp;};
    void setDinTemp(int t) { din_temp = t; reCalculate();};
    void setDinAirPressure(int p) {din_air_pressure = p; reCalculate();};
    qreal dinAirPressure() { return din_air_pressure; };
    void setDinTemp(qreal t) { din_temp=t; reCalculate();};
    void setDinAirPressure(qreal t) { din_air_pressure=t; reCalculate();};
    qreal getCarMass () {return mass;};
    void setCarMass (int nm) {mass = (qreal) nm; reCalculate();};
    int getSmoothAmount(){return smoothAmount; };
    void setSmoothAmount(int s) {smoothAmount=s; reCalculate();};
    double getDriveTrainLoss () { return driveTrainLoss;};
    void setDriveTrainLoss(double l) {driveTrainLoss=l; reCalculate();};

    void reCalculate(bool useGpsSpeed=false);

private:
    QwtPlotCurve *boostCurve;
    QwtPlotCurve *agtCurve;
    QwtPlotCurve *lambdaCurve;
    QwtPlotCurve *wheelPowerCurve;
    QwtPlotCurve *wheelTorqueCurve;
    QwtPlotCurve *dinPowerCurve;
    QwtPlotCurve *dinTorqueCurve;
    QwtPlotCurve *speedCurve;

    //	QVector<double> xboost;
    //	QVector<double> yboost;

    MdPlotData *boostData;
    MdPlotData *agtData;
    MdPlotData *lambdaData;
    MdPlotData *wheelPowerData;
    MdPlotData *wheelTorqueData;
    MdPlotData *dinPowerData;
    MdPlotData *dinTorqueData;
    MdPlotData *speedData;

    QHBoxLayout *myhorizontalLayout;

    //celsius
    qreal din_temp;
    //mbar
    qreal din_air_pressure;
    qreal mass;
    int smoothAmount;
    qreal driveTrainLoss;

    QList<MdDataRecord*> *dataListP;
    QList<int> rowNums;
    QMap<int,double> smoothedSpeed;
    QMap<int,double> smoothedRPM;
    QMap<int,double> smoothedTime;
};


#endif // POWERPLOT_H


