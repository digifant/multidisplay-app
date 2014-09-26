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


#include "MdPlot.h"
#include "MdPlotData.h"
#include "MdPlotZoomer.h"
#include "MdPlotPicker.h"
#include <QDebug>
#include <QGestureEvent>
#include <QSettings>
#include <limits>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>


MdPlot::MdPlot( QMainWindow* mw, QWidget* parent, QTableView *tableView ) {
    plotnameInSavedSettings = "MdPlot_default_name_change_me";

    myhorizontalLayout = new QHBoxLayout();
    //	myhorizontalLayout->setObjectName(QString::fromUtf8("myhorizontalLayout"));
    myhorizontalLayout->addWidget(this);
    parent->setLayout(myhorizontalLayout);

    myCfgDialog = new VisConfigDialog (mw);
    connect (myCfgDialog, SIGNAL(accepted()), this, SLOT(acceptCfgDialog()) );
    connect (myCfgDialog, SIGNAL(rejected()), this, SLOT(rejectCfgDialog()) );

    //taken from qwt examples

    // Zoom Mode:
    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size

    //Picker Mode:
    // Left: set Marker
    // Right: remove last Marker

    //TODO change TrackerWidget for dark background!

    d_zoomer[0] = 0;
    d_zoomer[1] = 0;
//    d_zoomer[0] = new MdPlotZoomer( QwtPlot::xBottom, QwtPlot::yLeft, this->canvas());
//    d_zoomer[0]->setRubberBand(QwtPicker::RectRubberBand);
//    d_zoomer[0]->setRubberBandPen(QColor(Qt::green));
//    d_zoomer[0]->setTrackerMode(QwtPicker::AlwaysOn);
//    d_zoomer[0]->setTrackerPen(QColor(Qt::white));

//    d_zoomer[1] = new MdPlotZoomer(QwtPlot::xTop, QwtPlot::yRight, this->canvas());

    d_panner = new QwtPlotPanner(this->canvas());
    d_panner->setMouseButton(Qt::MidButton);

    //qwt5
//    d_picker[0] = new MdPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
//                                   QwtPicker::PointSelection | QwtPicker::ClickSelection,
//                                   QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
//                                   this->canvas(), this);
    d_picker[0] = new MdPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                                       QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                                       dynamic_cast<QwtPlotCanvas*>(this->canvas()), this);

    d_picker[0]->setRubberBandPen(QColor(Qt::green));
    d_picker[0]->setTrackerMode(QwtPicker::AlwaysOn);
    d_picker[0]->setTrackerPen(QColor(Qt::white));
    //qwt5
//    d_picker[0]->setSelectionFlags(QwtPicker::PointSelection | QwtPicker::ClickSelection);
    d_picker[0]->setRemoveLastMarkerOnMouseRight(true);

    //qwt5
//    d_picker[1] = new MdPlotPicker(QwtPlot::xBottom, QwtPlot::yRight,
//                                   QwtPicker::PointSelection | QwtPicker::ClickSelection,
//                                   QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOff,
//                                   this->canvas(), this);
    d_picker[1] = new MdPlotPicker(QwtPlot::xBottom, QwtPlot::yRight,
                                       QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOff,
                                       dynamic_cast<QwtPlotCanvas*>(this->canvas()), this);
    d_picker[1]->setRubberBandPen(QColor(Qt::green));
    d_picker[1]->setTrackerPen(QColor(Qt::white));
    //qwt5.x
//    d_picker[1]->setSelectionFlags(QwtPicker::PointSelection | QwtPicker::ClickSelection);

    connect (d_picker[0], SIGNAL(selected (const QPointF &)), this, SLOT(pointSelected(const QPointF &)) );
    //    connect (d_picker[1], SIGNAL(selected (const QwtDoublePoint &)), this, SLOT(pointSelected(const QwtDoublePoint &)) );

//    connect (d_zoomer[0], SIGNAL( noZoom() ), this, SLOT( enableXBottomAutoScale() ) );
//    connect (d_zoomer[1], SIGNAL( noZoom() ), this, SLOT( enableXBottomAutoScale() ) );
    enableZoomMode(false);

    this->tableView = tableView;

#ifdef Q_WS_MAEMO_5
    //http://doc.qt.nokia.com/qt-maemo-4.6/gestures-overview.html
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::SwipeGesture);
#endif
}

MdPlot::~MdPlot() {
    // TODO Auto-generated destructor stub
}

bool MdPlot::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture) {
        qDebug() << "gesture";
        return gestureEvent(static_cast<QGestureEvent*>(event));
    }
    return QwtPlot::event(event);
}

bool MdPlot::gestureEvent(QGestureEvent *event)
{
    if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
//        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
        qDebug() << "swipe";
    else if (QGesture *pan = event->gesture(Qt::PanGesture))
//        panTriggered(static_cast<QPanGesture *>(pan));
        qDebug() << "pan";
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
//        pinchTriggered(static_cast<QPinchGesture *>(pinch));
        qDebug() << "pinch";
    return true;
}

QColor MdPlot::getCurveColor (int curvenum) {
    if (curvenum >= curveMap.size())
        return QColor(Qt::white);
    return (curveMap.values()[curvenum])->pen().color();
}

void MdPlot::pointSelected(const QPointF &pos) {
    qDebug() << "MdPlot::pointSelected x=" << pos.x() << " y=" << pos.y();
}

iResultList* MdPlot::getCurveValuesForXValue (const QPointF &pos ) {
    iResultList* l = new iResultList();
    bool search = true;
    iResult* first = NULL;
    foreach ( QwtPlotCurve* c, curveMap.values() ) {
        if ( c->isVisible() ) {
            MdPlotData* p = dynamic_cast<MdPlotData*> ( c->data() );
            iResult* i = new iResult();
            if ( search ) {
                p->interpolateSearch( pos.x(), *i);
                search = false;
                first = i;
            } else {
                i->needInterPolation = first->needInterPolation;
                i->l = first->l;
                i->r = first->r;
                i->x= first->x;
            }
            i->y = p->getInterpolatedYValue( i->l, i->r, i->x );
            i->curve = c;
            l->resultList.push_back(i);
        }
    }
    return l;
}


void MdPlot::showCfgDialog() {
    myCfgDialog->readConfig(curveMap);
    myCfgDialog->show();
}
void MdPlot::acceptCfgDialog() {
    qDebug() << "accept";
    for ( int i = 0 ; i < myCfgDialog->getCfgTableWidget()->rowCount() ; i++ ) {
        QString mdname = myCfgDialog->getCfgTableWidget()->item(i,0)->text();
        QString uiname = mdname;

        if ( myCfgDialog->getCfgTableWidget()->item(i,1) )
            uiname = myCfgDialog->getCfgTableWidget()->item(i,1)->text();

        qDebug() << myCfgDialog->getCfgTableWidget()->item(i,0)->text();
        //				<< " " << myCfgDialog->getCfgTableWidget()->item(i,1)->text() << " " << myCfgDialog->getCfgTableWidget()->item(i,2)->text() << " " << myCfgDialog->getCfgTableWidget()->item(i,3)->text() << " ";

        if ( curveMap.contains(mdname) ) {
            QwtPlotCurve* c = curveMap[mdname];
            if ( c ) {
                //set name
                c->setTitle(uiname);

                //en-/disable
                //legend
                QVariant plotItem = this->itemToInfo 	( c	);
                QwtAbstractLegend *al = this->legend();
                QwtLegend* legend = dynamic_cast<QwtLegend*> (al);
                if ( legend ) {
                    QWidget *lw = legend->legendWidget(plotItem);
                    if ( lw ) {
                        if ( myCfgDialog->getCfgTableWidget()->item(i,3)->checkState() ) {
                            lw->show();
                            c->setVisible(true);
                        } else {
                            c->setVisible(false);
                            lw->hide();
                        }
                    }
                }

                //set color
                if ( myCfgDialog->getCfgTableWidget()->item(i,2) ) {
                    QString cs = myCfgDialog->getCfgTableWidget()->item(i,2)->text();
                    QColor color (cs);
                    if ( color.isValid() )
                        c->setPen( QPen (color) );
                }
                replot();
            }

        }
    }
    //TODO set labels in the data view's column header

    //	QwtPlotCurve* pc = cfgMap[s];
}


void MdPlot::writeSettings () {
    QSettings settings("MultiDisplay", "UI");
    settings.beginGroup(plotnameInSavedSettings);
    foreach ( QString mdname, curveMap.keys() ) {
        QwtPlotCurve* c = curveMap[mdname];
        if ( c ) {
            settings.setValue (mdname+"_title", c->title().text() );
            settings.setValue (mdname+"_enabled", c->isVisible() );
            settings.setValue (mdname+"_color", c->pen().color().name());
        }
    }
    settings.endGroup();
}

void MdPlot::readSettings () {
    QSettings settings("MultiDisplay", "UI");

    settings.beginGroup(plotnameInSavedSettings);

    foreach ( QString mdname, curveMap.keys() ) {
    	bool enabled = settings.value (mdname + "_enabled", QVariant (false)).toBool();
    	if ( !enabled && ( (mdname == QString("rpm")) || (mdname == QString("boost"))
            || (mdname == QString("lambda")) || (mdname == QString("throttle")) ))
            enabled = true;
    	QString title = settings.value (mdname + "_title", QVariant ("")).toString();
    	if ( title.isEmpty() )
            title = mdname;
    	QString colors = settings.value (mdname + "_color", QVariant ("")).toString();
    	QColor color (colors);

        QwtPlotCurve* c = curveMap[mdname];
        if ( c ) {
            //set name
            c->setTitle(title);

            QVariant plotItem = this->itemToInfo 	( c	);
            QwtAbstractLegend *al = this->legend();
            QwtLegend* legend = dynamic_cast<QwtLegend*> (al);
            if ( legend ) {
                QWidget *lw = legend->legendWidget(plotItem);
                if ( lw ) {
                    if ( enabled )
                        lw->show();
                    else
                        lw->hide();
                }
            }

            if ( enabled ) {
                c->setVisible(true);
            } else {
                c->setVisible(false);
            }

            //set color
            if ( ! color.isValid() )
                // if no color is set in the config, use the color set in the constructor
                color = c->pen().color();

            if ( color.isValid() ) {
                c->setPen( QPen (color) );
            }
        }
    }
    settings.endGroup();
    replot();
}


void MdPlot::rejectCfgDialog() {
    qDebug() << "rejected";
}

void MdPlot::enableXBottomAutoScale() {
//    qDebug() << "MdPlot::enableXBottomAutoScale()";
    setAxisAutoScale(QwtPlot::xBottom);
    d_zoomer[0]->setZoomBase(false);
    d_zoomer[1]->setZoomBase(false);
    replot();
}



void MdPlot::toggleZoomMode() {
    enableZoomMode( ! d_panner->isEnabled() );
}

void MdPlot::enableZoomMode(bool on) {
    d_panner->setEnabled(on);

    if ( d_zoomer[0] && d_zoomer[1] ) {
        d_zoomer[0]->setEnabled(on);
        d_zoomer[0]->zoom(0);

        d_zoomer[1]->setEnabled(on);
        d_zoomer[1]->zoom(0);
    }

    d_picker[0]->setEnabled(!on);
    d_picker[1]->setEnabled(!on);

    //    showInfo();
}


void MdPlot::setWinSize (const int &nws) {
    foreach ( QwtPlotCurve* c, curveMap.values() ) {
        MdPlotData* d = dynamic_cast<MdPlotData*> ( c->data() );
        d->setWinSize(nws);
    }
    setXBottomScaling();
}
void MdPlot::setWinMarkMicroRelative (const int &quotient, const bool &left, const int &maxMark) {
    foreach ( QwtPlotCurve* c, curveMap.values() ) {
        MdPlotData* d = dynamic_cast<MdPlotData*> ( c->data() );
        d->setWinMarkMicroRelative(quotient, left, maxMark);
    }
    //	setXBottomScaling();
}
void MdPlot::setWinMark (const int &nwm, const int &maxMark) {
    foreach ( QwtPlotCurve* c, curveMap.values() ) {
        MdPlotData* d = dynamic_cast<MdPlotData*> ( c->data() );
        d->setWinMark(nwm, maxMark);
    }
//    setXBottomScaling();
}

void MdPlot::setXBottomScaling () {
//    qDebug() << "MdPlot::setXBottomScaling () ";
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::min();

//    bool reenableAutoScaling = this->axisAutoScale(QwtPlot::xBottom);

    foreach ( QwtPlotCurve* c, curveMap.values() ) {
        if ( c->isVisible() ) {
//            MdPlotData* d = dynamic_cast<MdPlotData*> ( c->data() );
            QwtSeriesData<QPointF>* d = c->data();
            Q_ASSERT (d);
            if ( d ) {
                QRectF br = d->boundingRect();
                if (br.left() < minX)
                    minX = br.left();
                if (br.right() > maxX)
                    maxX = br.right();
//                qDebug() << "left=" << br.left() << " minX=" << minX << "right=" << br.right() << " maxX=" << maxX;
            }
        }
    }
    setAxisScale(QwtPlot::xBottom, minX, maxX);
//    qDebug() << "setAxisScale left=" << minX << " right=" << maxX;

//    if ( reenableAutoScaling )
//        setAxisAutoScale(QwtPlot::xBottom);
}

void MdPlot::replot() {
    //use our own scaler
    setXBottomScaling();
    QwtPlot::replot();
}
