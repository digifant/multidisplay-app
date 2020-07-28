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


#include "MdPlotPicker.h"
#include "MdPlot.h"
#include "MdPlotData.h"
#include "VisualizationPlot.h"

#include <QDebug>
#include <qwt_picker_machine.h>

MdPlotPicker::MdPlotPicker (int xAxis, int yAxis, RubberBand rubberBand,
                DisplayMode trackerMode, QwtPlotCanvas* canvas, MdPlot* mdPlot )
        : QwtPlotPicker (xAxis, yAxis, rubberBand, trackerMode, canvas),
          myPlot(mdPlot), removeLastMarkerOnMouseRight(false) {

    //qwt6: removed selectionFlags because not longer supported from QwtPlotPicker()
    this->setStateMachine( new QwtPickerClickPointMachine() );
}

MdPlotPicker::~MdPlotPicker() {
        // TODO Auto-generated destructor stub
}

void MdPlotPicker::setRemoveLastMarkerOnMouseRight ( bool n ) {
	removeLastMarkerOnMouseRight = n;
}

//QwtText MdPlotPicker::trackerText (const QPoint &pos ) const {
//    qDebug() << "MdPlotPicker::trackerText pos=" << pos;
//    return QwtPlotPicker::trackerTextF (pos);
//}

QwtText MdPlotPicker::trackerTextF (const QPointF &pos ) const {
//        qDebug() << "MdPlotPicker::trackerTextF pos=" << pos;
        iResultList* l = myPlot->getCurveValuesForXValue ( pos );
        QwtText text;
        QString t;
        bool first = true;
        int curvenum = 0;
        foreach ( iResult *i, l->resultList) {
        	if ( i->curve->isVisible() ) {
                if ( first ) {
                	//changed from i->x to pos.x()
                	t.append ( QString::number(pos.x() ) );
                	first = false;
                }
                t.append( "|");
                t.append( "<span style=\"color:" );
//                t.append ( myPlot->getCurveColor(curvenum).name() );
                t.append ( i->curve->pen().color().name() );
                t.append ("\">");
                // show the real values (new apply a factor to some curves!
                bool gotit = false;
                if ( i->curve->title().text() == "Speed") {
                    t.append( QString::number(i->y / 10, 'f',1 ) );
                    gotit = true;
                }
                if ( i->curve->title().text() == "N75") {
                    t.append( QString::number(i->y * 25, 'f', 0) );
                    gotit = true;
                }
                if ( i->curve->title().text() == "Knock") {
                    t.append( QString::number(i->y * 25, 'f', 0) );
                    gotit = true;
                }
                if ( i->curve->title().text() == "Ignition Retard") {
                    t.append( QString::number(i->y * 4, 'f', 1) );
                    gotit = true;
                }
                if ( i->curve->title().text() == "Ignition") {
                    t.append( QString::number(i->y * 6, 'f', 1) );
                    gotit = true;
                }
                if ( i->curve->title().text() == "rpm") {
                    t.append( QString::number(i->y, 'f', 0) );
                    gotit = true;
                }
                if ( i->curve->title().text() == "lambda") {
                    t.append( QString::number(i->y, 'f', 2) );
                    gotit = true;
                }
                if ( ! gotit )
                    t.append( QString::number(i->y, 'f',1) );

                t.append( "</span>");
                curvenum++;
        	}
        }
        text.setText (t, QwtText::RichText );
        delete (l);
        return text;
}


void MdPlotPicker::widgetMousePressEvent ( QMouseEvent *e ) {
    if ( e->button() == Qt::RightButton && removeLastMarkerOnMouseRight ) {
        //				qDebug() << "MdPlotPicker::widgetMousePressEvent right";
        VisualizationPlot* v = dynamic_cast<VisualizationPlot*>(myPlot);
        if ( v )
            v->removeLastMarker();
    } else if ( e->button() == Qt::MiddleButton ) {
        qDebug() << " middle Button pressed!";
    }
//    if ( e->button() == Qt::LeftButton )
//        qDebug() << " left Button pressed!";

    QwtPlotPicker::widgetMousePressEvent (e);
}
