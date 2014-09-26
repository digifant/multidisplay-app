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

#ifndef EVALPLOTDATA_H_
#define EVALPLOTDATA_H_

#include <qwt_series_data.h>
#include <QVector>


class EvalPlotData : public QwtSeriesData<double> {
public:
	EvalPlotData();
	EvalPlotData( double xleft = 0, double xright = 1.8, double step = 0.01 );
	virtual ~EvalPlotData();

	size_t size() const;
    double sample (size_t i) const;
//	double x (size_t i) const;
//	double y (size_t i) const;
    QRectF   boundingRect () const;

private:
	QVector<double> xData;
	QVector<double> yData;

	double xleft;
	double xright;
	double step;

	unsigned int ecount;

};


class EvalPlotDataSimple : public QwtSeriesData<double> {
public:
	EvalPlotDataSimple();
	virtual ~EvalPlotDataSimple();

	size_t size() const;
    double sample (size_t i) const;
//	const double* x () ;
//	const double* y () ;
    QVector<double> x () const;
    QVector<double> y () const;
    QRectF boundingRect() const;
	void add (const double &x, const double &y);

private:
	QVector<double> xData;
	QVector<double> yData;

};

#endif /* EVALPLOTDATA_H_ */
