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


#ifndef MDPLOTDATA_H_
#define MDPLOTDATA_H_

#include <qwt_series_data.h>
#include <QVector>
#include <QList>

class QwtPlotCurve;

class iResult {
public:
	iResult();

	bool needInterPolation;
	int l;
	int r;
	double x;
	double y;
	QwtPlotCurve* curve;
};

class iResultList {
public:
	virtual ~iResultList ();

    QList<iResult*> resultList = QList<iResult*>();
};

/**
 * holds the data for our plots
 *
 * TODO: save as image
 * QImage image(1000, 1000, ...);
 * image.fill(Qt::transparent);
 * plot->print(image, ...);
 * image.save(...);
 */
class MdPlotData : public QwtSeriesData<QPointF> {
public:
	MdPlotData ();
	MdPlotData (int windowMark, int windowSize);
	MdPlotData (double xRange);
//	MyPlotData ( const MyPlotData& d);
	~MdPlotData ();

	size_t size() const;
    double x (size_t i) const;
    double y (size_t i) const { return sample (i).y(); };
    QPointF sample( size_t i ) const;

    //!hack
    void setY (size_t i, const double &val) { yData[i]=val; };
    QRectF 	boundingRect () const;
	void append (double x, double y);
	void cleanXLowerAs (double xDel);
	void clear ();
	//! new window size in seconds!
	void setWinSize(const int &nws);
        //! window mark in per cent: 0% is the newest record!
	void setWinMark(const int &nwm, const int &maxMark);
	void setWinMarkMicroRelative(const int &quotient, const bool &left, const int &maxMark);

	bool interpolateSearch ( const double xval, iResult &res );
	double getInterpolatedYValue ( int lindex, int rindex, double xval );

    int getWindowBegin();

    QVector<double> x() const ;
    QVector<double> y() const ;

private:
	void adjustWindow ();


	QVector<double> xData;
	QVector<double> yData;
	int cleanCounter;
	double xRange;

    //! right boundary
	int windowMark;
    //! left boundary = windowMark - windowSize
	int windowSize;

	//! start point of plotted data (lower vector index)
	int windowBegin;
	//! end point of plotted data (higher vector index)
	int windowEnd;
};

#endif /* MDPLOTDATA_H_ */
