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


#ifndef MDPLOTPICKER_H_
#define MDPLOTPICKER_H_

#include <qwt_plot_picker.h>
#include <qwt_plot_canvas.h>

class MdPlot;

class MdPlotPicker : public QwtPlotPicker {
public:
        MdPlotPicker();
        MdPlotPicker   ( int xAxis, int yAxis, RubberBand rubberBand,
                        DisplayMode trackerMode, QwtPlotCanvas* canvas, MdPlot *mdPlot );
        virtual ~MdPlotPicker();

        void setRemoveLastMarkerOnMouseRight ( bool n );

//        QwtText trackerText (const QPoint &pos ) const;
        QwtText trackerTextF ( const QPointF &pos )	const;


protected:
        virtual void widgetMousePressEvent ( QMouseEvent *e );

private:
        MdPlot* myPlot;
        bool removeLastMarkerOnMouseRight;
};

#endif /* MDPLOTPICKER_H_ */
