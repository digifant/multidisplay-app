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


#include "MdSpectrogramData.h"
#include <qdebug.h>

MdSpectrogramData::MdSpectrogramData() : xlower(0), xupper(0), ylower(0), yupper(0), step(0), data(0), maxVal(1) {

}


MdSpectrogramData::MdSpectrogramData( double xlower, double xupper, double ylower, double yupper, double step ) : xlower(xlower), xupper(xupper),
	ylower(ylower), yupper(yupper), step(step), maxVal(1)
{
	xcount = (xupper - xlower) / step;
	ycount = (yupper - ylower) / step;

	data = new QVector< QVector<double>* > ( xcount + 1, NULL );
	for ( int i = 0 ; i < data->size() ; i++ )
		(*data)[i] = new QVector<double> ( ycount + 1, NULL );

    setInterval( Qt::ZAxis, QwtInterval( 0.0, 10.0 ) );
}

MdSpectrogramData::~MdSpectrogramData() {
	// TODO delete the vectors !!!
}



QwtRasterData * MdSpectrogramData::copy() const {
//    return new MdSpectrogramData();
    return NULL;
}

QRectF MdSpectrogramData::boundingRect( ) const {
	if ( xcount  > 0 && ycount > 0 ) {
        return QRectF (xlower, ylower, xupper - xlower, yupper-ylower);
	}
    return QRectF(1.0, -1.0, 1.0, -1.0);
}

//QSize MdSpectrogramData::rasterHint ( const QwtDoubleRect &rect ) const {
//	qDebug() << "rasterHint " << rect << " returns " << QSize (xcount, ycount);
//	return QSize (xcount,ycount);
//}

QwtInterval MdSpectrogramData::interval( Qt::Axis axis ) const {
    return d_intervals[axis];
}

void MdSpectrogramData::setInterval( Qt::Axis axis, const QwtInterval &interval )
{
    d_intervals[axis] = interval;
}


double MdSpectrogramData::value(double x, double y) const {
    //qDebug() << "MdSpectrogram::value x=" << x << " y=" << y;
	if ( (x >= xlower) && (x <= xupper) && (y >= ylower) && (y <= yupper) ) {
		uint xindex = (x - xlower) / step;
		uint yindex = (y - ylower) / step;
//		return ((*data)[xindex])->at(yindex);
		double val = (*((*data)[xindex]))[yindex] ;
		double val2 = mapValue(val);
        //qDebug() << val2;
		return val2;
	}
	return 0;
}

//QwtDoubleInterval MdSpectrogramData::range () const {
//	//we use a fixed value range of [0;10]
//	return QwtDoubleInterval (0, 10);
//}

double MdSpectrogramData::mapValue ( const double &val ) const {
	return (val * 10) / maxVal;
}

void MdSpectrogramData::increment ( const double &x, const double &y ) {
	if ( (x >= xlower) && (x <= xupper) && (y >= ylower) && (y <= yupper) ) {
		uint xindex = (x - xlower) / step;
		uint yindex = (y - ylower) / step;
		double val = ++( (*((*data)[xindex]))[yindex]);
		if (val>maxVal)
			maxVal = val;
	}
}

void MdSpectrogramData::clear () {
	foreach ( QVector<double>* y , (*data) )
			if ( y )
				for ( int i = 0 ; i < y->size() ; i++ )
					(*y)[i] = 0;
	maxVal = 0;
}
