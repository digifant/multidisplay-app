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


#include <QDebug>
#include <QColor>
#include <qwt_symbol.h>

#include "EvalPlot.h"
#include "MdData.h"


EvalPlot::EvalPlot( QMainWindow* mw, QWidget *parent ) : MdPlot(mw, parent) {
//EvalPlot::EvalPlot( QMainWindow* mw, QWidget *parent ) : QwtPlot( parent ) {
//	qDebug() << "alive";
	ec = new QwtPlotCurve ("title");
	const QColor &c = Qt::red;
    ec->setSymbol( new QwtSymbol(QwtSymbol::XCross, QBrush(c), QPen(c), QSize(5, 5)) );
	ec->attach(this);

	setTitle ("Eval1");

	// Axis
	setAxisTitle(QwtPlot::xBottom, "Boost");
        setAxisScale(QwtPlot::xBottom, 0.0, 1.8);
	setAxisTitle(QwtPlot::yLeft, "Lambda");
	setAxisScale(QwtPlot::yLeft, 0, 2.0);
//	setAxisAutoScale(QwtPlot::xBottom);

	setCanvasBackground(Qt::black);
}

EvalPlot::~EvalPlot() {
	if ( ec )
		delete (ec);
}

void EvalPlot::clear() {
	//TODO
	qDebug() << "TODO implement me!";
}

void EvalPlot::addRecord(MdSensorRecord* r, bool doReplot) {
	Q_UNUSED(r);
	Q_UNUSED(doReplot);
	//not needed
}

void EvalPlot::replot() {
    QwtPlot::replot();
}

// ============================================================================================================================================

EvalPlotBoostLambda::EvalPlotBoostLambda( QMainWindow* mw, QWidget *parent ) : EvalPlot (mw, parent) {
	setTitle ("Boost / Lambda on WOT");
}

EvalPlotBoostLambda::~EvalPlotBoostLambda() {
}

void EvalPlotBoostLambda::compute ( MdData *md ) {
	QList<MdDataRecord*> dl = md->getData ();

	if ( dl.size() > 0 ) {
		foreach ( MdDataRecord* r , dl ) {
                    if ( r->getSensorR() != NULL ) {
			if ( r->getSensorR()->getThrottle() >= 90 )
				d.add (r->getSensorR()->getBoost(), r->getSensorR()->getLambda() );
                    }
		}
        ec->setSamples ( d.x(), d.y() );
//        ec->setSamples (&d);
	}
}


// ============================================================================================================================================

EvalPlotRPMBoost::EvalPlotRPMBoost( QMainWindow* mw, QWidget *parent ) : EvalPlot (mw, parent) {
	setTitle ("RPM / Boost on WOT");

	// Axis
        setAxisTitle(QwtPlot::xBottom, "RPM");
        setAxisScale(QwtPlot::xBottom, 0, 8000);
        setAxisTitle(QwtPlot::yLeft, "Boost");
        setAxisScale(QwtPlot::yLeft, -0.8, 2.0);

//	setAxisAutoScale(QwtPlot::xBottom);

}

EvalPlotRPMBoost::~EvalPlotRPMBoost() {
}

void EvalPlotRPMBoost::compute ( MdData *md ) {
	QList<MdDataRecord*> dl = md->getData ();

	if ( dl.size() > 0 ) {
		foreach ( MdDataRecord* r , dl ) {
                    if ( r->getSensorR() != NULL ) {
			if ( r->getSensorR()->getThrottle() >= 90 )
				d.add (r->getSensorR()->getRpm(), r->getSensorR()->getBoost() );
                    }
		}
        ec->setSamples( d.x(), d.y() );
	}
}

