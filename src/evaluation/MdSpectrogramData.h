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

#ifndef MDSPECTOGRAMDATA_H_
#define MDSPECTOGRAMDATA_H_

#include <qwt_raster_data.h>

class MdSpectrogramData : public QwtRasterData {
public:
	MdSpectrogramData ();
	MdSpectrogramData ( double xlower, double xupper, double ylower, double yupper, double step );
	virtual ~MdSpectrogramData();

    virtual QwtRasterData *copy() const;
//    virtual QwtDoubleInterval range() const;
    virtual double value(double x, double y) const;


    double mapValue ( const double &val ) const;
    virtual void increment ( const double &x, const double &y );

    void clear ();

    QRectF boundingRect ( ) const;
//    QSize rasterHint ( const QwtDoubleRect &rect ) const;

    double getMaxVal() { return maxVal; };

private:

	double xlower;
	double xupper;
	double ylower;
	double yupper;

	double step;

	QVector< QVector<double>* >* data;

	double maxVal;

	unsigned int xcount;
	unsigned int ycount;

};

#endif /* MDSPECTOGRAMDATA_H_ */
