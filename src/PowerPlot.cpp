#include <QDebug>

#include "PowerPlot.h"
#include "MdData.h"
#include "MdPlot.h"
#include "MdPlotData.h"
#include "math.h"
#include <qmath.h>
#include <qwt_legend.h>

PowerPlot::PowerPlot( QMainWindow* mw, QWidget* parent, QTableView *tableView ) :
    MdPlot(mw, parent, tableView), dataListP(NULL)
{
    din_temp = 20;
    din_air_pressure = 1013;
    mass = 1150;
    smoothAmount = 2;
//    driveTrainLoss = 0.763;
    driveTrainLoss = 0.79;

    configAxis();

//    setCanvasBackground(Qt::white);
    setCanvasBackground(Qt::black);


    boostData = new MdPlotData (0,200);
    agtData = new MdPlotData (0,200);
    lambdaData = new MdPlotData (0,200);
    wheelPowerData = new MdPlotData (0,200);
    wheelTorqueData = new MdPlotData (0,200);
    dinPowerData = new MdPlotData (0,200);
    dinTorqueData = new MdPlotData (0,200);
    speedData = new MdPlotData (0,200);

    setTitle (QString("Power"));


    boostCurve = new QwtPlotCurve ("boost");
    boostCurve->attach(this);
    boostCurve->setYAxis(QwtPlot::yRight);
    boostCurve->setSamples(dynamic_cast<QwtSeriesData<QPointF>* > (boostData) );
    boostCurve->setXAxis(QwtPlot::xTop);
    boostCurve->setVisible(true);

    agtCurve = new QwtPlotCurve ("EGT");
    agtCurve->attach(this);
    agtCurve->setYAxis(QwtPlot::yRight);
    agtCurve->setSamples(dynamic_cast<QwtSeriesData<QPointF>* > (agtData));
    agtCurve->setXAxis(QwtPlot::xTop);
    agtCurve->setVisible(true);

    lambdaCurve = new QwtPlotCurve ("Lambda");
    lambdaCurve->attach(this);
    lambdaCurve->setYAxis(QwtPlot::yRight);
    lambdaCurve->setSamples(dynamic_cast<QwtSeriesData<QPointF>* > (lambdaData));
    lambdaCurve->setXAxis(QwtPlot::xTop);
    lambdaCurve->setVisible(true);

    wheelPowerCurve = new QwtPlotCurve ("Wheel Power [KW]");
    wheelPowerCurve->attach(this);
    wheelPowerCurve->setYAxis(QwtPlot::yLeft);
    wheelPowerCurve->setXAxis(QwtPlot::xTop);
    wheelPowerCurve->setSamples(dynamic_cast<QwtSeriesData<QPointF>* > (wheelPowerData));
    wheelPowerCurve->setVisible(true);

    wheelTorqueCurve = new QwtPlotCurve ("Wheel Torque [Nm]");
    wheelTorqueCurve->attach(this);
    wheelTorqueCurve->setYAxis(QwtPlot::yLeft);
    wheelTorqueCurve->setSamples(dynamic_cast<QwtSeriesData<QPointF>* > (wheelTorqueData));
    wheelTorqueCurve->setXAxis(QwtPlot::xTop);
    wheelTorqueCurve->setVisible(true);

    dinPowerCurve = new QwtPlotCurve ("DIN Power [KW]");
    dinPowerCurve->attach(this);
    dinPowerCurve->setYAxis(QwtPlot::yLeft);
    dinPowerCurve->setXAxis(QwtPlot::xTop);
    dinPowerCurve->setSamples(dynamic_cast<QwtSeriesData<QPointF>* > (dinPowerData));
    dinPowerCurve->setVisible(true);

    dinTorqueCurve = new QwtPlotCurve ("DIN Torque [Nm]");
    dinTorqueCurve->attach(this);
    dinTorqueCurve->setYAxis(QwtPlot::yLeft);
    dinTorqueCurve->setSamples(dynamic_cast<QwtSeriesData<QPointF>* > (dinTorqueData));
    dinTorqueCurve->setXAxis(QwtPlot::xTop);
    dinTorqueCurve->setVisible(true);

    speedCurve = new QwtPlotCurve ("Speed");
    speedCurve->attach(this);
    speedCurve->setYAxis(QwtPlot::yLeft);
    speedCurve->setSamples(dynamic_cast<QwtSeriesData<QPointF>* > (speedData));
    speedCurve->setXAxis(QwtPlot::xTop);
    speedCurve->setVisible(true);

    wheelPowerCurve->setPen(QPen(Qt::darkRed));
    wheelTorqueCurve->setPen(QPen(Qt::darkYellow));
    dinPowerCurve->setPen(QPen(Qt::red));
    dinTorqueCurve->setPen(QPen(Qt::yellow));
    lambdaCurve->setPen(QPen(Qt::blue));
    agtCurve->setPen(QPen(Qt::magenta));
    speedCurve->setPen(QPen(Qt::darkYellow));
    boostCurve->setPen(QPen(Qt::green));
    boostCurve->setStyle( QwtPlotCurve::Lines );

    QwtLegend *legend = new QwtLegend();
    //    legend->setItemMode(QwtLegend::CheckableItem);
    insertLegend(legend, QwtPlot::BottomLegend);
}

void PowerPlot::configAxis() {
    // Axis
    setAxisTitle(QwtPlot::xBottom, "RPM");
    setAxisScale(QwtPlot::xBottom, 1500,9000);
    enableAxis(QwtPlot::xBottom, true);
    setAxisScale(QwtPlot::xTop, 1500,9000);
    enableAxis(QwtPlot::xTop, true);
//    setAxisAutoScale(QwtPlot::xBottom);
    setAxisTitle(QwtPlot::yLeft, "Power, Torque, EGT(x0.5), Speed");
    setAxisScale(QwtPlot::yLeft, 0, 800);
    setAxisTitle(QwtPlot::yRight, "Lambda, Boost");
    setAxisScale(QwtPlot::yRight, 0, 2.5);
    enableAxis(QwtPlot::yRight, true);
}

void PowerPlot::setData ( QList<MdDataRecord*>&dl, QList<int> &rn, bool useGpsSpeed ) {
    dataListP = &dl;
    rowNums = rn;

    //clean data -> remove rows without changing speed!
    qSort (rowNums.begin(), rowNums.end());
    QList<int> removeIdx;
    for ( quint32 i = 0 ; i < rowNums.size() ; i++ ) {
        if ( i>0) {
            if ( useGpsSpeed ) {
                if ( dataListP->at(rowNums.at(i-1))->getMobileR()->gpsGroundSpeed == dataListP->at(rowNums.at(i))->getMobileR()->gpsGroundSpeed )
                    removeIdx.append(i);
            } else {
                if ( dataListP->at(rowNums.at(i-1))->getSensorR()->getSpeed() == dataListP->at(rowNums.at(i))->getSensorR()->getSpeed() )
                    removeIdx.append(i);
            }
        }
    }
    qDebug() << "setData clean: removeIdx " << removeIdx;
    foreach (int r , removeIdx)
        rowNums.removeAt(r);

    if ( useGpsSpeed ) {
        removeIdx.clear();
        for ( quint32 i = 0 ; i < rowNums.size() ; i++ ) {
            if ( i>0) {
                if ( dataListP->at(rowNums.at(i-1))->getSensorR()->getSpeed() == dataListP->at(rowNums.at(i))->getSensorR()->getSpeed() )
                    removeIdx.append(i);
            }
        }
        qDebug() << "setData clean: removeIdx " << removeIdx;
        foreach (int r , removeIdx)
            rowNums.removeAt(r);
    }
    smoothedSpeed = smoothSpeedMovingAverage(useGpsSpeed);
    smoothedRPM = smoothRPMMovingAverage();
    smoothedTime = smoothTimeMovingAverage();
    reCalculate(useGpsSpeed);
}

void PowerPlot::smoothMovingAverage (MdPlotData* d, const quint8 smooth) {
    if ( smooth == 0 )
        return;

    QMap<int,double> smoothed;

    for ( quint32 i = 0 ; i < d->size() ; i++ ) {
            double sum=0;
            for ( int j = (-1*smooth) ; j <= smooth ; j++ ) {
                int z = i + j;
                if ( z < 0 )
                    z=0;
                if ( z >= d->size() )
                    z=d->size()-1;
                sum += d->y(z);
            }
            smoothed [i] = (double) (sum / (2.0*smooth + 1));
//            qDebug() << "smooth original " << d->y(i) << " smoothed=" << smoothed[i];
    }

    foreach (int i, smoothed.keys())
        d->setY (i, smoothed[i]);
}

QMap<int,double> PowerPlot::smoothSpeedMovingAverage (bool useGpsSpeed) {
    QMap<int,double> res;
    for ( quint32 i = 0 ; i < rowNums.size() ; i++ ) {
            double sum=0;
            for ( int j = -smoothAmount ; j <= smoothAmount ; j++ ) {
                int z = i + j;
                if ( z < 0 )
                    z=0;
                if ( z >= rowNums.size() )
                    z=rowNums.size()-1;
                if ( useGpsSpeed )
                    sum += dataListP->at(rowNums.at(z))->getMobileR()->gpsGroundSpeed;
                else
                    sum += dataListP->at(rowNums.at(z))->getSensorR()->getSpeed();
            }
            res[rowNums.at(i)] = sum / (2.0*smoothAmount + 1);
            if ( smoothAmount == 0 && useGpsSpeed )
                res[rowNums.at(i)] = dataListP->at(rowNums.at(i))->getMobileR()->gpsGroundSpeed;
            if ( smoothAmount == 0 && !useGpsSpeed )
                res[rowNums.at(i)] = dataListP->at(rowNums.at(i))->getSensorR()->getSpeed();
            double delta = 0;
            double deltas = 0;
            if ( useGpsSpeed && i >= 1 ) {
                delta = dataListP->at(rowNums.at(i))->getMobileR()->gpsGroundSpeed - dataListP->at(rowNums.at(i-1))->getMobileR()->gpsGroundSpeed;
                deltas = res[rowNums.at(i)] - res[rowNums.at(i-1)];
            } else if ( i >= 1 ) {
                delta = dataListP->at(rowNums.at(i))->getSensorR()->getSpeed() - dataListP->at(rowNums.at(i-1))->getSensorR()->getSpeed();
                deltas = res[rowNums.at(i)] - res[rowNums.at(i-1)];
            }
//            if ( useGpsSpeed )
//                qDebug() << i << " smooth original speed=" << dataListP->at(rowNums.at(i))->getMobileR()->gpsGroundSpeed << " d=" << delta << " smoothed=" << res[rowNums.at(i)] << " d=" << deltas;
//            else
//                qDebug() << i << " smooth original speed=" << dataListP->at(rowNums.at(i))->getSensorR()->getSpeed() << " d=" << delta <<  " smoothed=" << res[rowNums.at(i)] << " d=" << deltas;
    }
    return res;
}

QMap<int,double> PowerPlot::smoothRPMMovingAverage () {
    QMap<int,double> res;
    for ( quint32 i = 0 ; i < rowNums.size() ; i++ ) {
            double sum=0;
            for ( int j = -smoothAmount ; j <= smoothAmount ; j++ ) {
                int z = i + j;
                if ( z < 0 )
                    z=0;
                if ( z >= rowNums.size() )
                    z=rowNums.size()-1;
                sum += dataListP->at(rowNums.at(z))->getSensorR()->getRpm();
            }
            res[rowNums.at(i)] = sum / (2.0*smoothAmount + 1);
            if ( smoothAmount == 0)
                res[rowNums.at(i)] = dataListP->at(rowNums.at(i))->getSensorR()->getRpm();
            double delta = 0;
            double deltas = 0;
            if ( i >= 1 ) {
                delta = dataListP->at(rowNums.at(i))->getSensorR()->getRpm() - dataListP->at(rowNums.at(i-1))->getSensorR()->getRpm();
                deltas = res[rowNums.at(i)] - res[rowNums.at(i-1)];
            }
//            qDebug() << i << " smooth original rpm=" << dataListP->at(rowNums.at(i))->getSensorR()->getRpm() << " d=" << delta <<  " smoothed=" << res[rowNums.at(i)] << " d=" << deltas;
    }
    return res;
}

QList<double> PowerPlot::smoothMovingAverage (const QList<double> &data, const quint8 smooth) {
    QList<double> res;
    for ( quint32 i = 0 ; i < data.size() ; i++ ) {
            double sum=0;
            for ( int j = -smooth ; j <= smooth ; j++ ) {
                int z = i + j;
                if ( z < 0 )
                    z=0;
                if ( z >= data.size() )
                    z=data.size()-1;
                sum += data.at(z);
            }
            res.append (sum / (2.0*smooth + 1));
            double delta = 0;
            double deltas = 0;
            if ( i >= 1 ) {
                delta = data.at(i) - data.at(i-1);
                deltas = res.at(i) - res.at(i-1);
            }
//            qDebug() << i << " smooth original data=" << data.at(i) << " d=" << delta <<  " smoothed=" << res.at(i) << " d=" << deltas;
    }
    return res;
}

QMap<int,double> PowerPlot::smoothTimeMovingAverage () {
    QMap<int,double> res;
    for ( quint32 i = 0 ; i < rowNums.size() ; i++ ) {
            double sum=0;
            for ( int j = -smoothAmount ; j <= smoothAmount; j++ ) {
                int z = i + j;
                if ( z < 0 )
                    z=0;
                if ( z >= rowNums.size() )
                    z=rowNums.size()-1;
                sum += dataListP->at(rowNums.at(z))->getSensorR()->getTime();
            }
            res[rowNums.at(i)] = sum / (2.0*smoothAmount + 1);
            if ( smoothAmount == 0 )
                res[rowNums.at(i)] = dataListP->at(rowNums.at(i))->getSensorR()->getTime();
            double delta = 0;
            double deltas = 0;
            if ( i >= 1 ) {
                delta = dataListP->at(rowNums.at(i))->getSensorR()->getTime() - dataListP->at(rowNums.at(i-1))->getSensorR()->getTime();
                deltas = res[rowNums.at(i)] - res[rowNums.at(i-1)];
            }
//            qDebug() << i << " smooth original time=" << dataListP->at(rowNums.at(i))->getSensorR()->getTime() << " d=" << delta <<  " smoothed=" << res[rowNums.at(i)] << " d=" << deltas;
    }
    return res;
}
void PowerPlot::reCalculate( bool useGpsSpeed ) {
    clear();
    ComputedPowerData *cp = new ComputedPowerData();
    quint32 uc = 0;
    quint32 lastUsedRow = 0;

    double dinMaxTorque=0;
    double dinMaxTorqueRPM=0;
    double dinMaxPower=0;
    double dinMaxPowerRPM=0;
    double wheelMaxTorque=0;
    double wheelMaxTorqueRPM=0;
    double wheelMaxPower=0;
    double wheelMaxPowerRPM=0;

    double unsmoothed_dinMaxTorque=0;
    double unsmoothed_dinMaxTorqueRPM=0;
    double unsmoothed_dinMaxPower=0;
    double unsmoothed_dinMaxPowerRPM=0;


    double rpmDelta = 0;

    QList<double> rpm;

    foreach (quint32 r, rowNums) {
        MdDataRecord *cur = dataListP->at(r);

        MdDataRecord *last = cur;
        if ( lastUsedRow > 0 )
            last = dataListP->at(lastUsedRow);
        else
            lastUsedRow = r;

//        int curRpm = cur->getSensorR()->getRpm();
        int curRpm = smoothedRPM[r];
//        rpmDelta = cur->getSensorR()->getRpm() - last->getSensorR()->getRpm();
//        qDebug() << "rpm " << curRpm << " delta=" << rpmDelta;

//        uc = cur->getMobileR()->gpsUpdateCount;

        computePower (cur,last, cp, lastUsedRow, r, useGpsSpeed);

        if ( ! isnan(cp->kw_engine) ) {
            boostData->append (curRpm, cur->getSensorR()->getBoost());
            agtData->append (curRpm, cur->getSensorR()->getEgt4() / 2);

            if ( useGpsSpeed ) {
//                speedData->append (curRpm, cur->getMobileR()->gpsGroundSpeed);
                speedData->append (curRpm, smoothedSpeed[r]);
            } else {
//                speedData->append (curRpm, cur->getSensorR()->getSpeed());
                speedData->append (curRpm, smoothedSpeed[r]);
            }

            lambdaData->append (curRpm, cur->getSensorR()->getLambda());

            dinTorqueData->append( curRpm, cp->torque_engine_din);
            wheelTorqueData->append( curRpm, cp->torque_wheel);
            dinPowerData->append( curRpm, cp->kw_engine_din);
            wheelPowerData->append( curRpm, cp->kw_wheel);
            rpm.append(curRpm);
        }

        lastUsedRow = r;
    }

    if ( cp )
        delete (cp);

    //find max values
    for (int i=0 ; i < dinPowerData->size() ; i++) {
        if ( dinPowerData->y(i) > unsmoothed_dinMaxPower ) {
            unsmoothed_dinMaxPower=dinPowerData->y(i);
            unsmoothed_dinMaxPowerRPM=dinPowerData->x(i);
        }
        if ( dinTorqueData->y(i) > unsmoothed_dinMaxTorque ) {
            unsmoothed_dinMaxTorque=dinTorqueData->y(i);
            unsmoothed_dinMaxTorqueRPM=dinTorqueData->x(i);
        }
    }


    smoothMovingAverage(wheelPowerData, smoothAmount);
    smoothMovingAverage(wheelTorqueData, smoothAmount);
    smoothMovingAverage(dinPowerData, smoothAmount);
    smoothMovingAverage(dinTorqueData, smoothAmount);
    smoothMovingAverage(boostData, smoothAmount);
    smoothMovingAverage(lambdaData, smoothAmount);
    smoothMovingAverage(agtData, smoothAmount);

    //find max values
    for (int i=0 ; i < dinPowerData->size() ; i++) {
        qDebug() << dinPowerData->x(i) << " RPM " << dinPowerData->y(i) << " KW " << dinTorqueData->y(i) << " Nm "
                 << " | " << wheelPowerData->y(i) << " KW " << wheelTorqueData->y(i) << " Nm " ;

        if ( dinPowerData->y(i) > dinMaxPower ) {
            dinMaxPower=dinPowerData->y(i);
            dinMaxPowerRPM=dinPowerData->x(i);
        }
        if ( wheelPowerData->y(i) > wheelMaxPower ) {
            wheelMaxPower=wheelPowerData->y(i);
            wheelMaxPowerRPM=wheelPowerData->x(i);
        }
        if ( wheelTorqueData->y(i) > wheelMaxTorque ) {
            wheelMaxTorque=wheelTorqueData->y(i);
            wheelMaxTorqueRPM=wheelTorqueData->x(i);
        }
        if ( dinTorqueData->y(i) > dinMaxTorque ) {
            dinMaxTorque=dinTorqueData->y(i);
            dinMaxTorqueRPM=dinTorqueData->x(i);
       }
    }

    QString maxValuesString = "DIN engine max Torque " + QString::number(dinMaxTorque, 'f', 2) + "Nm @ " + QString::number(dinMaxTorqueRPM,'f',0)
            + " max power " + QString::number(dinMaxPower,'f',2) + "KW @ " + QString::number(dinMaxPowerRPM,'f',0) + " | wheel "
            + "max Torque " + QString::number(wheelMaxTorque,'f',2) + "Nm @ " + QString::number(wheelMaxTorqueRPM,'f',0)
            + " max power " + QString::number(wheelMaxPower,'f',2) + "KW @ " + QString::number(wheelMaxPowerRPM,'f',0);
    qDebug() << maxValuesString;
    emit resultString(maxValuesString);

    qDebug() << "unsmoothed raw values: max Engine Torque " << unsmoothed_dinMaxTorque << "@" << unsmoothed_dinMaxTorqueRPM << " max engine DIN power [kw] " << unsmoothed_dinMaxPower << "@" << unsmoothed_dinMaxPowerRPM;

    replot();
}

void PowerPlot::clear() {
    boostData->clear();
    agtData->clear();
    speedData->clear();
    dinTorqueData->clear();
    dinPowerData->clear();
    wheelTorqueData->clear();
    wheelPowerData->clear();
    lambdaData->clear();
    replot();
}

void PowerPlot::computePower (MdDataRecord* curR, MdDataRecord* lastR, ComputedPowerData* cp, int lastRowNr, int rowNr, bool useGpsSpeed) {
    if ( cp == NULL )
        return;

    MdSensorRecord* cur = curR->getSensorR();
    MdSensorRecord* last = lastR->getSensorR();

    qreal start_velocity = 0;
    qreal end_velocity = 0;
    if ( useGpsSpeed ) {
        start_velocity = lastR->getMobileR()->gpsGroundSpeed;
        end_velocity = curR->getMobileR()->gpsGroundSpeed;
    } else {
        start_velocity = last->getSpeed();
        end_velocity = cur->getSpeed();
    }

    //contains the unsmoothed speed if smoothAmount is 0 !
    if ( lastRowNr > -1 && rowNr > -1 ) {
        //use smoothed speed
        start_velocity = smoothedSpeed[lastRowNr];
        end_velocity = smoothedSpeed[rowNr];
    }

    //original time
    qreal time_s = (cur->getTime() - last->getTime()) / 1000.0;
    //smoothed time
    if ( lastRowNr > -1 && rowNr > -1 ) {
        //use smoothed speed
        time_s = (smoothedTime[rowNr] - smoothedTime[lastRowNr]) / 1000.0;
    }

    //m/s^2
    //use accelerometer data from N900 ?
    qreal acceleration = (end_velocity - start_velocity) * (0.27778) / (time_s);
    qreal force = mass * acceleration;
    //distance travelled
    qreal distance = (start_velocity * 0.27778 * time_s) +  1/2 * acceleration * time_s * time_s;
    //work
    qreal work = force * distance;
    qreal power = work / time_s;
//                    qDebug() << "start time=" << last->getSensorR()->getTime() << " end time=" << cur->getSensorR()->getTime();
//                    qDebug() << "start velocity " << QString::number(start_velocity, 'f', 4) << " end_velocity=" << QString::number(end_velocity, 'f', 4) << " time=" << QString::number(time_s, 'g', 2)
//                                << " distance=" << QString::number(distance, 'g', 2) << " acceleration=" << QString::number(acceleration, 'g', 2) << " force=" << QString::number(force, 'g', 2) << " worK="
//                                << QString::number(work, 'f', 2) << " power=" << QString::number(power, 'f', 2);

    //luftwiderstand
    /* Fr = A/2 * Cw * D * v²
    A=Stirnfläche, Cw=Cw Wert, D=Dichte der Luft (abhängig von Temperatur)
    cw-Wert bei 195er 0,32 bei 205er und Plusachse 0,34 (ist aber nicht sicher!!)
    Stirnflächen: normaler Corrado 1,81m² US und 2.0L (kleine Frontlippe) 1,80m²
    */
    qreal f_air = 1.81*0.5 * 0.34 * 1.29 * qPow(end_velocity*0.27778, 2);
    qreal w_air = f_air * distance;
    qreal p_air = w_air / time_s;

    /*
      Rollwiderstand
    Fr = µr * m * g
    [N]=[ ] [kg] [m/s²]

    µr = Rollwiderstandsbeiwert
    Rollwiderstand 0,015 drin 215/40
    195/50 Rollwiderstand 0,013

    gueltig auch bei hohen Geschwindigkeiten > 150km/h???
    */
    qreal f_roll = 0.015 * mass * 9.81;
    qreal w_roll = f_roll * distance;
    qreal p_roll = w_roll / time_s;

    qreal wpower = power + p_roll + p_air;
    qreal wtorque = wpower / (2*M_PI * (cur->getRpm()/60) );
    qreal epower = wpower * (1/driveTrainLoss);
    qreal etorque = epower / (2*M_PI * (cur->getRpm()/60) );
    //Korrekturfaktor = (1013 / Atmosphärendruck) x Wurzel aus [(273 + Temperatur am Prüfstand) / (273 + 20)]
    qreal dinpower = epower * ((1013 / din_air_pressure) * qSqrt ((273 + din_temp) / (273 + 20)));
    qreal dintorque = (dinpower)/(2*M_PI * (cur->getRpm()/60) );

//    QString tip2 = "Power (wheel) " + QString::number( wpower/1000, 'f', 2)
//            + " (kW) " + QString::number( (wpower*1.34)/1000, 'f', 2 ) + " (Hp/PS)"
//            + "\n" + "Engine Power "  + QString::number( epower/1000, 'f', 2)
//            + " (kW) " + QString::number( (epower*1.34)/1000, 'f', 2 ) + " (Hp/PS)"
//            + "\n" + "Engine Power DIN 70020 "  + QString::number( dinpower/1000, 'f', 2)
//            + " (kW) " + QString::number( (dinpower*1.34)/1000, 'f', 2 ) + " (Hp/PS)" + " " + QString::number(torque, 'f', 2) + "Nm";
//    qDebug() << tip2;

    cp->kw_wheel = wpower / 1000;
    cp->torque_wheel = wtorque;
    cp->kw_engine = epower / 1000;
    cp->torque_engine = etorque;
    cp->kw_engine_din = dinpower / 1000;
    cp->torque_engine_din = dintorque;
    return;
}

void PowerPlot::setXBottomScaling() {
    configAxis();
}

qreal PowerPlot::interpolate (qreal x1, qreal y1, qreal x2, qreal y2, qreal xs) {
//    if ( xs < x1 )
//        return y1;
//    if ( xs > x1 )
//        return y2;
    return y1 + ( ( (y2-y1) / (x2-x1) ) * (xs - x1) );
}

qreal PowerPlot::findX (qreal x1, qreal y1, qreal x2, qreal y2, qreal ys) {
     return (ys - y1 + ( (y2-y1) / (x2-x1) ) * x1) / ( (y2-y1) / (x2-x1) ) ;
}

QMap<qreal,SpeedData> PowerPlot::calculateTimeBetweenSpeeds ( QList<MdDataRecord*>&dl, QList<int> &rn, qreal startspeed, qreal endspeed ) {
    QMap<qreal,SpeedData> timeTable;
    timeTable[-1] = SpeedData();

    if ( rn.size() < 2 )
        return timeTable;

    int idx_startl = 0;
    int idx_starth = 0;
    int idx_endl = 0;
    int idx_endh = 0;

    //sort ascending
    qSort (rn.begin(), rn.end());

    bool upperFound = false;
    foreach ( int i, rn ) {
        if ( dl.at(i)->getSensorR()->getSpeed() < startspeed ) {
            idx_startl = i;
            idx_starth = i+1;
        }
        if ( dl.at(i)->getSensorR()->getSpeed() < endspeed && !upperFound ) {
            idx_endl = i;
            idx_endh = i+1;
            if ( dl.at(idx_endh)->getSensorR()->getSpeed() > endspeed )
                upperFound = true;
        }
    }
    if ( idx_endh >= dl.size() ) {
        qDebug() << "calculateTimeBetweenSpeeds out of bounds";
        return timeTable;
    }
    if ( idx_starth >= dl.size() ) {
        qDebug() << "calculateTimeBetweenSpeeds out of bounds | start not found!";
        return timeTable;
    }
    if ( dl.at(idx_starth)->getSensorR()->getSpeed() < startspeed ) {
        qDebug() << "calculateTimeBetweenSpeeds error no upper start speed found!";
        return timeTable;
    }
    if ( dl.at(idx_endh)->getSensorR()->getSpeed() < endspeed ) {
        qDebug() << "calculateTimeBetweenSpeeds error no upper end speed found!";
        return timeTable;
    }
    qreal startMillis = findX ( dl.at(idx_startl )->getSensorR()->getTime(), dl.at(idx_startl )->getSensorR()->getSpeed(),
                                dl.at(idx_starth )->getSensorR()->getTime(), dl.at(idx_starth )->getSensorR()->getSpeed(),
                                startspeed );
    qreal endMillis = findX ( dl.at(idx_endl )->getSensorR()->getTime(), dl.at(idx_endl )->getSensorR()->getSpeed(),
                                dl.at(idx_endh )->getSensorR()->getTime(), dl.at(idx_endh )->getSensorR()->getSpeed(),
                                endspeed );
    qreal startRPM = interpolate ( dl.at(idx_startl )->getSensorR()->getTime(), dl.at(idx_startl )->getSensorR()->getRpm(),
                                   dl.at(idx_starth )->getSensorR()->getTime(), dl.at(idx_starth )->getSensorR()->getRpm(),
                                   startMillis );
    qreal endRPM = interpolate ( dl.at(idx_endl )->getSensorR()->getTime(), dl.at(idx_endl )->getSensorR()->getRpm(),
                                   dl.at(idx_endh )->getSensorR()->getTime(), dl.at(idx_endh )->getSensorR()->getRpm(),
                                   startMillis );
    qreal time = endMillis - startMillis;
    qDebug() << "calculateTimeBetweenSpeeds startspeed=" << startspeed << " endspeed=" << endspeed << " " << time << " msecs"
             << " startRPM=" << startRPM << " endRPM=" << endRPM;
    timeTable = calculateAdditionalDataTimeBetweenSpeeds(dl, rn, 100, 200, false);
    SpeedData ts;
    ts.time_s=time/1000.0;
    timeTable[-1]=ts;
    return timeTable;
}

QMap<qreal, SpeedData> PowerPlot::calculateAdditionalDataTimeBetweenSpeeds( QList<MdDataRecord*>&dl, QList<int> &rn, qreal speedL, qreal speedH, bool useGps ) {
    QMap<qreal,SpeedData> timeTable;
    //sort ascending
    qSort (rn.begin(), rn.end());

    qreal first_time = 0;
    qreal alt_old = 0;
    qreal time_old = 0;

    for ( qreal s = speedL ; s <= speedH ; s = s+10 ) {
        int idx_l = 0;
        int idx_h = 0;

        bool upperFound = false;
        foreach ( int i, rn ) {
            if ( ( useGps ? ( dl.at(i)->getMobileR()->gpsGroundSpeed < s ) : ( dl.at(i)->getSensorR()->getSpeed() < s ) ) && !upperFound ) {
                idx_l = i;
                idx_h = i+1;
                if ( useGps ? ( dl.at(idx_h)->getMobileR()->gpsGroundSpeed > s ) : ( dl.at(idx_h)->getSensorR()->getSpeed() > s ) )
                    upperFound = true;
            }
        }
        if ( idx_h >= dl.size() ) {
            return timeTable;
        }
        qreal cm = findX ( dl.at(idx_l )->getSensorR()->getTime(), useGps ? (dl.at(idx_l )->getMobileR()->gpsGroundSpeed) : (dl.at(idx_l )->getSensorR()->getSpeed()),
                                    dl.at(idx_h )->getSensorR()->getTime(), useGps ? (dl.at(idx_h )->getMobileR()->gpsGroundSpeed) : (dl.at(idx_h )->getSensorR()->getSpeed()),
                                    s );
        qreal alt = interpolate ( dl.at(idx_l )->getSensorR()->getTime(), dl.at(idx_l )->getMobileR()->gpsAltitude,
                                    dl.at(idx_h )->getSensorR()->getTime(), dl.at(idx_h )->getMobileR()->gpsAltitude,
                                    cm );
        qreal time = 0;

        if (alt_old == 0)
                alt_old = alt;

        if ( first_time == 0 ) {
            first_time = cm;
            time_old = cm;
        } else
            time = cm - first_time;

        SpeedData sd;
        sd.time_s = time / 1000.0;
        sd.time_delta = (cm - time_old) / 1000;
        sd.alt = alt;
        sd.alt_delta = alt_old - alt;
        alt_old = alt;
        timeTable[s] = sd;
        time_old = cm;
    }
    return timeTable;
}

QMap<qreal,SpeedData> PowerPlot::calculateTimeBetweenSpeedsGPS ( QList<MdDataRecord*>&dl, QList<int> &rn, qreal startspeed, qreal endspeed ) {
    QMap<qreal,SpeedData> timeTable;
    timeTable[-1]=SpeedData();

    if ( rn.size() < 2 )
        return timeTable;

    int idx_startl = 0;
    int idx_starth = 0;
    int idx_endl = 0;
    int idx_endh = 0;

    //sort ascending
    qSort (rn.begin(), rn.end());

    bool upperFound = false;
    foreach ( int i, rn ) {
        if ( dl.at(i)->getMobileR()->gpsGroundSpeed < startspeed ) {
            idx_startl = i;
            idx_starth = i+1;
        }
        if ( dl.at(i)->getMobileR()->gpsGroundSpeed < endspeed && !upperFound ) {
            idx_endl = i;
            idx_endh = i+1;
            if ( dl.at(idx_endh)->getMobileR()->gpsGroundSpeed > endspeed )
                upperFound = true;
        }
    }
    if ( idx_endh >= dl.size() ) {
        qDebug() << "calculateTimeBetweenSpeeds out of bounds";
        return timeTable;
    }
    if ( idx_starth >= dl.size() ) {
        qDebug() << "calculateTimeBetweenSpeeds out of bounds | start not found!";
        return timeTable;
    }
    if ( dl.at(idx_starth)->getMobileR()->gpsGroundSpeed < startspeed ) {
        qDebug() << "calculateTimeBetweenSpeeds error no upper start speed found!";
        return timeTable;
    }
    if ( dl.at(idx_endh)->getMobileR()->gpsGroundSpeed < endspeed ) {
        qDebug() << "calculateTimeBetweenSpeeds error no upper end speed found!";
        return timeTable;
    }
    qreal startMillis = findX ( dl.at(idx_startl )->getSensorR()->getTime(), dl.at(idx_startl )->getMobileR()->gpsGroundSpeed,
                                dl.at(idx_starth )->getSensorR()->getTime(), dl.at(idx_starth )->getMobileR()->gpsGroundSpeed,
                                startspeed );
    qreal endMillis = findX ( dl.at(idx_endl )->getSensorR()->getTime(), dl.at(idx_endl )->getMobileR()->gpsGroundSpeed,
                                dl.at(idx_endh )->getSensorR()->getTime(), dl.at(idx_endh )->getMobileR()->gpsGroundSpeed,
                                endspeed );
    qreal startRPM = interpolate ( dl.at(idx_startl )->getSensorR()->getTime(), dl.at(idx_startl )->getSensorR()->getRpm(),
                                   dl.at(idx_starth )->getSensorR()->getTime(), dl.at(idx_starth )->getSensorR()->getRpm(),
                                   startMillis );
    qreal endRPM = interpolate ( dl.at(idx_endl )->getSensorR()->getTime(), dl.at(idx_endl )->getSensorR()->getRpm(),
                                   dl.at(idx_endh )->getSensorR()->getTime(), dl.at(idx_endh )->getSensorR()->getRpm(),
                                   startMillis );
    qreal time = endMillis - startMillis;
    qDebug() << "calculateTimeBetweenSpeeds startspeed=" << startspeed << " endspeed=" << endspeed << " " << time << " msecs"
             << " startRPM=" << startRPM << " endRPM=" << endRPM;
    timeTable = calculateAdditionalDataTimeBetweenSpeeds(dl, rn, 100, 200, true);
    SpeedData ts;
    ts.time_s=time/1000.0;
    timeTable[-1]=ts;
    return timeTable;
}
