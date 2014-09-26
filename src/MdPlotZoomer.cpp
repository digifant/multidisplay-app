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


#include "MdPlotZoomer.h"
#include <QDebug>

MdPlotZoomer::MdPlotZoomer(int xAxis, int yAxis, QwtPlotCanvas *canvas) :
QwtPlotZoomer(xAxis, yAxis, canvas), myc(0)
{
    //selectionFlags missing in qwt 6.x
//        setSelectionFlags(QwtPicker::DragSelection | QwtPicker::CornerToCorner);
//        setTrackerMode(QwtPicker::AlwaysOff);


        //        setTrackerMode(QwtPicker::AlwaysOn);
        setRubberBand(QwtPicker::NoRubberBand);

        // RightButton: zoom out by 1
        // Ctrl+RightButton: zoom out to full size

#if QT_VERSION < 0x040000
        setMousePattern(QwtEventPattern::MouseSelect2,
                        Qt::RightButton, Qt::ControlButton);
#else
        setMousePattern(QwtEventPattern::MouseSelect2,
                        Qt::RightButton, Qt::ControlModifier);
#endif
        setMousePattern(QwtEventPattern::MouseSelect3,
                        Qt::RightButton);
}

void MdPlotZoomer::zoom (const QRectF &rect ) {
        QwtPlotZoomer::zoom (rect);

        myc++;
        qDebug() << "MdPlotZoomer::zoom myc=" << myc << "left=" << rect.left() << " top=" << rect.top()
                                        << " right=" << rect.right() << " bottom=" << rect.bottom();
}

void MdPlotZoomer::zoom (int up) {
        QwtPlotZoomer::zoom (up);

        myc = myc + up;
        qDebug() << "MdPlotZoomer::zoom up=" << up << " myc=" << myc;
        if ( (up == 0) || (myc == 0) ) {
                myc = 0;
                qDebug() << "zoomed to 0";
                emit noZoom();
        }
}

