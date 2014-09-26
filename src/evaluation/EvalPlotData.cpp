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

#include "EvalPlotData.h"


EvalPlotData::EvalPlotData () : ecount (0) {
}


EvalPlotData::EvalPlotData (double xleft, double xright, double step) : xleft(xleft), xright(xright), step(step)  {
	ecount = (xright - xleft) / step;
	xData.resize(ecount + 1);
	yData.resize(ecount + 1);

	double x = xleft;
	for ( int i = 0 ; i < ecount ; i++ ) {
		xData[i]=x;
		yData[i]=0;
		x += step;
	}
}

EvalPlotData::~EvalPlotData() {
	// TODO Auto-generated destructor stub
}

size_t EvalPlotData::size() const {
	return ecount;

}
//double EvalPlotData::x (size_t i) const {
//	if ( i < ecount )
//		return xData[i];
//	return -666;
//}
double EvalPlotData::sample (size_t i) const {
	if ( i < ecount )
		return yData[i];
	return -666;
}

QRectF EvalPlotData::boundingRect() const {
	if ( xData.size() > 0  ) {
        return QRectF (xData[0], 0, xData[ecount-1] - xData[0], 0);
	}
    return QRectF(1.0, -1.0, 1.0, -1.0);
}


// =============================================================================================================================

EvalPlotDataSimple::EvalPlotDataSimple() {
}

EvalPlotDataSimple::~EvalPlotDataSimple() {
}

size_t EvalPlotDataSimple::size() const {
	return xData.size();
}

QVector<double> EvalPlotDataSimple::x () const {
    return xData;
}
QVector<double> EvalPlotDataSimple::y () const {
    return yData;
}


//double EvalPlotDataSimple::x (size_t i) const {
//	if ( (i >= 0) && (i < xData.size()) )
//		return xData[i];
//	return -666;
//}
double EvalPlotDataSimple::sample (size_t i) const {
	if ( (i>=0) && (i < xData.size()) )
		return yData[i];
	return -666;
}

QRectF EvalPlotDataSimple::boundingRect () const {
	if ( xData.size() > 0  ) {
		//TODO
        return QRectF (xData[0], 0, xData[xData.size()-1] - xData[0], 0);
	}
    return QRectF(1.0, -1.0, 1.0, -1.0);
}


void EvalPlotDataSimple::add (const double &x, const double &y) {
	xData.resize( xData.size() + 1 );
	yData.resize( yData.size() + 1 );
	xData[xData.size()-1] = x;
	yData[yData.size()-1] = y;
}
