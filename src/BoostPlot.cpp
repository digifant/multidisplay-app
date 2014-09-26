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


#include "BoostPlot.h"
#include <QPen>
#include <QColor>
#include <qdebug.h>
#include <qwt_legend.h>

BoostPidPlot::BoostPidPlot (QMainWindow* mw, QWidget *parent ) : MdPlot(mw, parent) {
        boostData = new MdPlotData (0,50);
        setPointData = new MdPlotData (0,50);
        mapPwmData = new MdPlotData (0,50);
        outputData = new MdPlotData (0,50);
        throttleData = new MdPlotData (0,50);
        aggData = new MdPlotData (0,50);

        setTitle (QString("Boost Plot"));

        boostCurve = new QwtPlotCurve ("boost");
        boostCurve->attach(this);

        setPointCurve = new QwtPlotCurve ("SetPoint");
        setPointCurve->attach(this);

        mapPwmCurve = new QwtPlotCurve ("PWM map");
        mapPwmCurve->attach(this);
        mapPwmCurve->setYAxis(QwtPlot::yRight);

        outputCurve = new QwtPlotCurve ("PWM Out");
        outputCurve->attach(this);
        outputCurve->setYAxis(QwtPlot::yRight);

        throttleCurve = new QwtPlotCurve ("Throttle");
        throttleCurve->attach(this);

        aggCurve = new QwtPlotCurve ("aggressive mode");
        aggCurve->attach(this);

        QPen rp = QPen(Qt::red);
        rp.setWidth(2);
//	boostCurve->setPen(QPen(Qt::red));
        boostCurve->setPen(rp);
        setPointCurve->setPen(QPen(Qt::yellow));
        mapPwmCurve->setPen(QPen(Qt::white));
        outputCurve->setPen(QPen(Qt::blue));
        throttleCurve->setPen(QPen(Qt::green));
        aggCurve->setPen(QPen(Qt::magenta));
        boostCurve->setStyle( QwtPlotCurve::Lines );

        // Axis
        setAxisTitle(QwtPlot::xBottom, "Time/milli-seconds");
        setAxisScale(QwtPlot::xBottom, 0, 100000);
        setAxisTitle(QwtPlot::yLeft, "Boost");
        setAxisScale(QwtPlot::yLeft, -0.5, 2.5);
        setAxisTitle(QwtPlot::yRight, "PWM duty");
        setAxisScale(QwtPlot::yRight, 0, 280);
        enableAxis(QwtPlot::yRight, true);
        setAxisAutoScale(QwtPlot::xBottom);

        setCanvasBackground(Qt::black);

        boostCurve->setData( boostData );
        setPointCurve->setSamples ( setPointData );
        outputCurve->setSamples ( outputData );
        mapPwmCurve->setSamples( mapPwmData );
        throttleCurve->setSamples ( throttleData );
        aggCurve->setSamples( aggData );

        insertLegend(new QwtLegend(), QwtPlot::BottomLegend);

        setAxisAutoScale(QwtPlot::xBottom);

        setCanvasBackground(Qt::black);

        curveMap["boost"] = boostCurve;
        curveMap["setpoint"] = setPointCurve;
        curveMap["throttle"] = throttleCurve;
        curveMap["output"] = outputCurve;
        curveMap["mapPWM"] = mapPwmCurve;
        curveMap["aggressive mode"] = aggCurve;
}



BoostPidPlot::~BoostPidPlot() {
	// TODO Auto-generated destructor stub
}


void BoostPidPlot::addRecord ( MdSensorRecord* r, bool doReplot ) {
	boostData->append(r->getTime(), r->getBoost() );
        setPointData->append (r->getTime(), r->getN75ReqBoost() );
        outputData->append (r->getTime(), r->getN75() );
	throttleData->append(r->getTime(), r->getThrottle()/100 );
        mapPwmData->append(r->getTime(), r->getN75ReqBoostPWM() );
        aggData->append(r->getTime(), (r->getFlags() & 2) );
	if ( doReplot )
		replot();
}

void BoostPidPlot::clear () {
	boostData->clear();
	setPointData->clear();
	outputData->clear();
	throttleData->clear();
	replot();
}





