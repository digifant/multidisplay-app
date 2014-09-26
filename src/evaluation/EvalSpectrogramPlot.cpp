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


#include "EvalSpectrogramPlot.h"
#include "MdSpectrogramData.h"
#include <qwt_color_map.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>
#include <qdebug.h>

#include "mdutil.h"

EvalSpectrogramPlot::EvalSpectrogramPlot( QMainWindow* mw, QWidget *parent ) : EvalPlot(mw, parent), sampleIntervall(0) {

    ec->detach();

    es = new QwtPlotSpectrogram();

    colorMap = new QwtLinearColorMap(Qt::black, Qt::red);
    colorMap->addColorStop(0.2, Qt::gray);
    colorMap->addColorStop(0.6, Qt::green);
    colorMap->addColorStop(0.95, Qt::yellow);

    es->setColorMap( (QwtColorMap*) colorMap);

	setAxisTitle(QwtPlot::xBottom, "Boost");
	setAxisScale(QwtPlot::xBottom, 0, 1.6);
	setAxisTitle(QwtPlot::yLeft, "Lambda");
	setAxisScale(QwtPlot::yLeft, 0.69, 1.34);



    data = new MdSpectrogramData(0, 1.6, 0.69, 1.34, 0.01);
    es->setData( data );
    es->attach(this);

    QList<double> contourLevels;
    for ( double level = 0.5; level < 10.0; level += 1.0 )
        contourLevels += level;
    es->setContourLevels(contourLevels);

    // A color bar on the right axis
    QwtScaleWidget *rightAxis = axisWidget(QwtPlot::yRight);
    rightAxis->setTitle("Intensity");
    rightAxis->setColorBarEnabled(true);
//    rightAxis->setColorMap(es->data()->range(),
//        es->colorMap());

//    setAxisScale(QwtPlot::yRight,
//        es->data().range().minValue(),
//        es->data().range().maxValue() );
    enableAxis(QwtPlot::yRight);

    plotLayout()->setAlignCanvasToScales(true);

    es->setDisplayMode(QwtPlotSpectrogram::ImageMode, true);
    // QPen(Qt::NoPen)
    es->setDefaultContourPen( QPen() );


//    replot();

//    // LeftButton for the zooming
//    // MidButton for the panning
//    // RightButton: zoom out by 1
//    // Ctrl+RighButton: zoom out to full size
//
//    QwtPlotZoomer* zoomer = new MyZoomer(canvas());
//#if QT_VERSION < 0x040000
//    zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
//        Qt::RightButton, Qt::ControlButton);
//#else
//    zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
//        Qt::RightButton, Qt::ControlModifier);
//#endif
//    zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
//        Qt::RightButton);
//
//    QwtPlotPanner *panner = new QwtPlotPanner(canvas());
//    panner->setAxisEnabled(QwtPlot::yRight, false);
//    panner->setMouseButton(Qt::MidButton);
//
//    // Avoid jumping when labels with more/less digits
//    // appear/disappear when scrolling vertically
//
//    const QFontMetrics fm(axisWidget(QwtPlot::yLeft)->font());
//    QwtScaleDraw *sd = axisScaleDraw(QwtPlot::yLeft);
//    sd->setMinimumExtent( fm.width("100.00") );
//
//    const QColor c(Qt::darkBlue);
//    zoomer->setRubberBandPen(c);
//    zoomer->setTrackerPen(c);

}

EvalSpectrogramPlot::~EvalSpectrogramPlot() {
	if (es)
		delete es;
    if (data)
        delete data;
    if (colorMap)
        delete colorMap;
}


void EvalSpectrogramPlot::compute ( MdData *md ) {
	QList<MdDataRecord*> dl = md->getData ();

    data = (MdSpectrogramData*) ( es->data() );
	data->clear();

	//calc sample intervall!
	long time = 0;
        qint32 samples = 0;

	if ( dl.size() > 0 ) {
		foreach ( MdDataRecord* r , dl ) {
                    if ( r->getSensorR() != NULL ) {
			if ( r->getSensorR()->getThrottle() >= 90 ) {
//				double rpm = round_nplaces( r->getSensorR()->getRpm(), 2);
				time += r->getSensorR()->getTime();
				double boost = round_nplaces( r->getSensorR()->getBoost(), 2) ;
				double lambda = round_nplaces( r->getSensorR()->getLambda(), 2);
				data->increment(boost, lambda);
//				qDebug() << boost << " " << lambda << " value=" << data->value(boost,lambda);
                                samples++;
			}
                    }
		}
                sampleIntervall = time / samples; //msecs
                qDebug() << "EvalSpectrogramPlot::compute finished: sampleIntervall=" << sampleIntervall  << " msecs" << " samples=" << samples;

//        qDebug()<<"bounding rect = " << es->data()->boundingRect();
		replot();
	}
}
