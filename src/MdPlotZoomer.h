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

#ifndef MDPLOTZOOMER_H_
#define MDPLOTZOOMER_H_

#include <qwt_plot_zoomer.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_canvas.h>



class MdPlotZoomer : public QwtPlotZoomer {
        Q_OBJECT

public:
        MdPlotZoomer(int xAxis, int yAxis, QwtPlotCanvas *canvas);
        virtual void zoom ( const QRectF &rect );
    virtual void zoom (int up);

signals:
        void noZoom ();

private:
    int myc;
};

#endif /* MDPLOTZOOMER_H_ */
