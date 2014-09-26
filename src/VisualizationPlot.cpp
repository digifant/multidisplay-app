/*
    Copyright 2010 Stephan Martin, Dominik Gummel

    This file is part of Multidisplay.

    Multidisplay is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Multidisplay is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Multidisplay.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "VisualizationPlot.h"
#include "MdPlotPicker.h"

#include <QPen>
#include <QColor>
#include <qdebug.h>
#include <qwt_legend.h>
#include <qwt_plot_marker.h>



VisualizationPlot::VisualizationPlot(QMainWindow* mw, QWidget *parent, QTableView *tableView )
    : MdPlot(mw, parent, tableView) {

    plotnameInSavedSettings = "Vis1";

    boostData = new MdPlotData (0, 100);
    rpmData = new MdPlotData (0, 100);
    lambdaData = new MdPlotData (0, 100);
    throttleData = new MdPlotData (0, 100);
    egt0Data = new MdPlotData (0, 100);
    egt1Data = new MdPlotData (0, 100);
    egt2Data = new MdPlotData (0, 100);
    egt3Data = new MdPlotData (0, 100);
    egt4Data = new MdPlotData (0, 100);
    egt5Data = new MdPlotData (0, 100);
    VDOPres1Data = new MdPlotData (0, 100);
    VDOPres2Data = new MdPlotData (0, 100);
    VDOPres3Data = new MdPlotData (0, 100);
    VDOTemp1Data = new MdPlotData (0, 100);
    VDOTemp2Data = new MdPlotData (0, 100);
    VDOTemp3Data = new MdPlotData (0, 100);

    lmmData = new MdPlotData (0, 100);
    speedData = new MdPlotData (0, 100);
    gearData = new MdPlotData (0, 100);
    n75Data = new MdPlotData (0, 100);

    //	setTitle (QString("Boost / RPM / Lambda / Throttle / EGT"));

    //#ifdef Q_WS_MAEMO_5
    //        QwtText t = title();
    //        t.setColor( Qt::white );
    //        setTitle(t);
    //#endif

    boostCurve = new QwtPlotCurve ("boost");
    boostCurve->attach(this);

    rpmCurve = new QwtPlotCurve ("RPM");
    rpmCurve->attach(this);
    rpmCurve->setYAxis(QwtPlot::yRight);

    lambdaCurve = new QwtPlotCurve ("Lambda");
    lambdaCurve->attach(this);

    throttleCurve = new QwtPlotCurve ("Throttle");
    throttleCurve->attach(this);

    egt0Curve = new QwtPlotCurve ("EGT 0");
    egt0Curve->attach(this);
    egt0Curve->setYAxis(QwtPlot::yRight);

    egt1Curve = new QwtPlotCurve ("EGT 1");
    egt1Curve->attach(this);
    egt1Curve->setYAxis(QwtPlot::yRight);

    egt2Curve = new QwtPlotCurve ("EGT 2");
    egt2Curve->attach(this);
    egt2Curve->setYAxis(QwtPlot::yRight);

    egt3Curve = new QwtPlotCurve ("EGT 3");
    egt3Curve->attach(this);
    egt3Curve->setYAxis(QwtPlot::yRight);

    egt4Curve = new QwtPlotCurve ("EGT 4");
    egt4Curve->attach(this);
    egt4Curve->setYAxis(QwtPlot::yRight);

    egt5Curve = new QwtPlotCurve ("EGT 5");
    egt5Curve->attach(this);
    egt5Curve->setYAxis(QwtPlot::yRight);

    VDOTemp1Curve = new QwtPlotCurve ("VDO Temp1");
    VDOTemp1Curve->attach(this);
    VDOTemp1Curve->setYAxis(QwtPlot::yRight);
    VDOTemp1Curve->setVisible(false);

    VDOTemp2Curve = new QwtPlotCurve ("VDO Temp2");
    VDOTemp2Curve->attach(this);
    VDOTemp2Curve->setYAxis(QwtPlot::yRight);
    VDOTemp2Curve->setVisible(false);

    VDOTemp3Curve = new QwtPlotCurve ("VDO Temp3");
    VDOTemp3Curve->attach(this);
    VDOTemp3Curve->setYAxis(QwtPlot::yRight);
    VDOTemp3Curve->setVisible(false);

    VDOPres1Curve = new QwtPlotCurve ("VDO Pres 1");
    VDOPres1Curve->attach(this);
    VDOPres1Curve->setYAxis(QwtPlot::yRight);
    VDOPres1Curve->setVisible(false);

    VDOPres2Curve = new QwtPlotCurve ("VDO Pres 2");
    VDOPres2Curve->attach(this);
    VDOPres2Curve->setYAxis(QwtPlot::yRight);
    VDOPres2Curve->setVisible(false);


    VDOPres3Curve = new QwtPlotCurve ("VDO Pres 3");
    VDOPres3Curve->attach(this);
    VDOPres3Curve->setYAxis(QwtPlot::yRight);
    VDOPres3Curve->setVisible(false);

    lmmCurve = new QwtPlotCurve ("LMM");
    lmmCurve->attach(this);
    lmmCurve->setYAxis(QwtPlot::yRight);

    speedCurve = new QwtPlotCurve ("Speed");
    speedCurve->attach(this);
    speedCurve->setYAxis(QwtPlot::yRight);

    gearCurve = new QwtPlotCurve ("Gear");
    gearCurve->attach(this);
    gearCurve->setYAxis(QwtPlot::yLeft);

    n75Curve = new QwtPlotCurve ("N75");
    n75Curve->attach(this);
    n75Curve->setYAxis(QwtPlot::yLeft);

    boostCurve->setPen(QPen(Qt::red));
    rpmCurve->setPen(QPen(Qt::yellow));
    lambdaCurve->setPen(QPen(Qt::blue));
    throttleCurve->setPen(QPen(Qt::green));
    egt0Curve->setPen(QPen(Qt::magenta));
    egt1Curve->setPen(QPen(Qt::cyan));
    egt2Curve->setPen(QPen(Qt::darkMagenta));
    egt3Curve->setPen(QPen(Qt::darkCyan));
    egt4Curve->setPen(QPen(Qt::darkBlue));
    egt5Curve->setPen(QPen(Qt::white));
    VDOTemp1Curve->setPen(QPen(QColor("#FFFFFF")));
    VDOTemp2Curve->setPen(QPen(QColor("#FFFFFF")));
    VDOTemp3Curve->setPen(QPen(QColor("#FFFFFF")));
    VDOPres1Curve->setPen(QPen(QColor("#FFFFFF")));
    VDOPres2Curve->setPen(QPen(QColor("#FFFFFF")));
    VDOPres3Curve->setPen(QPen(QColor("#FFFFFF")));
    lmmCurve->setPen(QPen(Qt::darkRed));
    speedCurve->setPen(QPen(Qt::darkYellow));
    gearCurve->setPen(QPen(Qt::gray));
    n75Curve->setPen(QPen(Qt::darkGray));

    curveMap["boost"] = boostCurve;
    curveMap["rpm"] = rpmCurve;
    curveMap["throttle"] = throttleCurve;
    curveMap["lambda"] = lambdaCurve;
    curveMap["egt0"] = egt0Curve;
    curveMap["egt1"] = egt1Curve;
    curveMap["egt2"] = egt2Curve;
    curveMap["egt3"] = egt3Curve;
    curveMap["egt4"] = egt4Curve;
    curveMap["egt5"] = egt5Curve;
    curveMap["VDOTemp1"] = VDOTemp1Curve;
    curveMap["VDOTemp2"] = VDOTemp2Curve;
    curveMap["VDOTemp3"] = VDOTemp3Curve;
    curveMap["VDOPres1"] = VDOPres1Curve;
    curveMap["VDOPres2"] = VDOPres2Curve;
    curveMap["VDOPres3"] = VDOPres3Curve;
    curveMap["LMM"] = lmmCurve;
    curveMap["Speed"] = speedCurve;
    curveMap["gear"] = gearCurve;
    curveMap["N75"] = n75Curve;

    boostCurve->setStyle( QwtPlotCurve::Lines );

    // Axis
    setAxisTitle(QwtPlot::xBottom, "Time / minutes");
    setAxisScale(QwtPlot::xBottom, 0, 100000);
    setAxisTitle(QwtPlot::yLeft, "Boost / Throttle / Lambda");
    setAxisScale(QwtPlot::yLeft, -1.0, 12.0);
    setAxisTitle(QwtPlot::yRight, "RPM");
    setAxisScale(QwtPlot::yRight, 0, 7500);
    enableAxis(QwtPlot::yRight, true);
    setAxisAutoScale(QwtPlot::xBottom);

    setCanvasBackground(Qt::black);

    //TODO change cursor for black background!
    canvas()->setCursor(QCursor(Qt::ArrowCursor));

    boostCurve->setSamples( dynamic_cast<QwtSeriesData<QPointF>* > (boostData) );
    rpmCurve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (rpmData) );
    lambdaCurve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (lambdaData) );
    throttleCurve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (throttleData) );
    egt0Curve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (egt0Data) );
    egt1Curve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (egt1Data) );
    egt2Curve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (egt2Data) );
    egt3Curve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (egt3Data) );
    egt4Curve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (egt4Data) );
    egt5Curve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (egt5Data) );
    VDOTemp1Curve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (VDOTemp1Data) );
    VDOTemp2Curve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (VDOTemp2Data)  );
    VDOTemp3Curve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (VDOTemp3Data) );
    VDOPres1Curve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (VDOPres1Data) );
    VDOPres2Curve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (VDOPres2Data) );
    VDOPres3Curve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (VDOPres3Data) );

    lmmCurve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (lmmData) );
    speedCurve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (speedData) );
    gearCurve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (gearData) );
    n75Curve->setSamples ( dynamic_cast<QwtSeriesData<QPointF>* > (n75Data) );

#ifndef Q_WS_MAEMO_5
    QwtLegend *legend = new QwtLegend();
    //    legend->setItemMode(QwtLegend::CheckableItem);
    insertLegend(legend, QwtPlot::BottomLegend);

    connect(this, SIGNAL(legendChecked(QwtPlotItem *, bool)), SLOT(showCurve(QwtPlotItem *, bool)));
#endif
}

VisualizationPlot::~VisualizationPlot() {
    // TODO Auto-generated destructor stub
}

void VisualizationPlot::addRecord(MdSensorRecord *r, bool doReplot) {
    if ( r ) {

        //we apply a factor to some data for better fitting to the y axis
        //-> undo this at MdPlotPicker.cpp line 60!

        boostData->append(r->getTime()/60000.0, r->getBoost() );
        rpmData->append (r->getTime()/60000.0, r->getRpm() );
        lambdaData->append (r->getTime()/60000.0, r->getLambda() );
        throttleData->append(r->getTime()/60000.0, r->getThrottle()/100.0 );
        egt0Data->append(r->getTime()/60000.0, r->getEgt0() );
        egt1Data->append(r->getTime()/60000.0, r->getEgt1() );
        egt2Data->append(r->getTime()/60000.0, r->getEgt2() );
        egt3Data->append(r->getTime()/60000.0, r->getEgt3() );
        egt4Data->append(r->getTime()/60000.0, r->getEgt4() );
        egt5Data->append(r->getTime()/60000.0, r->getEgt5() );

        VDOTemp1Data->append ( r->getTime()/60000.0, r->getVDOTemp1() );
        VDOTemp2Data->append ( r->getTime()/60000.0, r->getVDOTemp2() );
        VDOTemp3Data->append ( r->getTime()/60000.0, r->getVDOTemp3() );
        VDOPres1Data->append ( r->getTime()/60000.0, r->getVDOPres1() );
        VDOPres2Data->append ( r->getTime()/60000.0, r->getVDOPres2() );
        VDOPres3Data->append ( r->getTime()/60000.0, r->getVDOPres3() );

        //0-5V
        //FIXME map to air mass
        lmmData->append ( r->getTime()/60000.0, r->getLmm() );

        speedData->append ( r->getTime()/60000.0, r->getSpeed() * 10 );
        gearData->append ( r->getTime()/60000.0, r->getGear() );
        //255 is 10 on left axis
        n75Data->append ( r->getTime()/60000.0, r->getN75() * 0.04 );

        if ( doReplot && this->isVisible() ) {
            //              updateAxes();
            replot();
        }
    }
}


void VisualizationPlot::pointSelected(const QPointF &pos) {
    quint32 millis = pos.x() * 60000;
#if  !defined (Q_WS_MAEMO_5)  && !defined (ANDROID)
    if ( tableView ) {
        MdData* md = dynamic_cast<MdData*> (tableView->model());
        if ( md ) {
            QModelIndex i = md->findRowForMillis( millis );
            tableView->setCurrentIndex( i );
        }
    }
#endif
    qDebug() << "VisualizationPlot::pointSelected x=" << pos.x() << " y=" << pos.y() << " millisecs=" << millis;
    QwtPlotMarker *m = new QwtPlotMarker();
    m->setLineStyle(QwtPlotMarker::VLine);
    m->setXValue(pos.x());
    if ( markerList.size() % 2 == 0 )
        m->setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
    else
        m->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
    m->setLabelOrientation(Qt::Horizontal);
    m->setLabel ((d_picker[0])->trackerTextF(pos));
    m->setLinePen(QPen(Qt::white, 0, Qt::DashDotLine));
    m->attach(this);
    markerList.push_back(m);
    markerMillisecsList.append (millis);
    replot();
}

void VisualizationPlot::removeLastMarker () {
    if ( !markerList.empty() ) {
        QwtPlotMarker *m = markerList.last();
        markerList.pop_back();
        markerMillisecsList.pop_back();
        int millis = 0;
        if ( ! markerMillisecsList.isEmpty() )
            millis = markerMillisecsList.last();
        m->detach();
        delete (m);
        replot();
#if  !defined (Q_WS_MAEMO_5)  && !defined (ANDROID)
        if ( tableView ) {
            MdData* md = dynamic_cast<MdData*> (tableView->model());
            if ( md ) {
                QModelIndex i = md->findRowForMillis( millis );
                qDebug() << " show millis = " << millis;
                tableView->setCurrentIndex( i );
            }
        }
#endif
    }
}

void VisualizationPlot::clear () {
    boostData->clear();
    rpmData->clear();
    lambdaData->clear();
    throttleData->clear();
    egt0Data->clear();
    egt1Data->clear();
    egt2Data->clear();
    egt3Data->clear();
    lmmData->clear();
    speedData->clear();
    gearData->clear();
    n75Data->clear();
    foreach (QwtPlotMarker* m, markerList) {
        m->detach();
        delete (m);
    }
    markerList.clear();
    updateAxes();
    replot();
}

void VisualizationPlot::enableXBottomAutoScale() {
    setAxisScale(QwtPlot::yLeft, -1.5, 2.0);
    setAxisScale(QwtPlot::yRight, 0, 7500);

    MdPlot::enableXBottomAutoScale();
}

//void VisualizationPlot::toggleShowAGT0 () {
//	egt0Curve->setVisible(!(egt0Curve->isVisible()));
//	replot();
//}
//void VisualizationPlot::toggleShowAGT1 () {
//	egt1Curve->setVisible(!(egt1Curve->isVisible()));
//	replot();
//}
//
//void VisualizationPlot::showAGT0( bool visible) {
//	egt0Curve->setVisible(visible);
//	replot();
//}
//void VisualizationPlot::showAGT1( bool visible) {
//	egt1Curve->setVisible(visible);
//	replot();
//}


void VisualizationPlot::showCurve(QwtPlotItem *item, bool on) {
    item->setVisible(!on);
    //qwt6 ?
//    QWidget *w = legend()->find(item);
//    if ( w && w->inherits("QwtLegendItem") )
//        ((QwtLegendItem *)w)->setChecked(on);

    replot();
}

int VisualizationPlot::windowBegin() {
    return boostData->getWindowBegin();
}
