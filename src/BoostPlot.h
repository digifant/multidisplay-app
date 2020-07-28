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

#ifndef BOOSTPLOT_H_
#define BOOSTPLOT_H_

#include "ui_multidisplayuimainwindow.h"

#include "MdData.h"
#include "MdPlotData.h"
#include "MdPlot.h"

#include <QWidget>
#include <QHBoxLayout>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>



class BoostPidPlot : public MdPlot {
    Q_OBJECT

public:
    BoostPidPlot( QMainWindow* mw, QWidget *parent );
    virtual ~BoostPidPlot();

    virtual void addRecord ( MdSensorRecord* r, bool doReplot=true );
    void clear ();

private:
    Ui::MultidisplayUIMainWindowClass *ui;
    QwtPlotCurve *boostCurve;
    QwtPlotCurve *setPointCurve;
    QwtPlotCurve *mapPwmCurve;
    QwtPlotCurve *outputCurve;
    QwtPlotCurve *throttleCurve;
    QwtPlotCurve *aggCurve;
    //	QVector<double> xboost;
    //	QVector<double> yboost;

    MdPlotData *boostData;
    MdPlotData *setPointData;
    MdPlotData *mapPwmData;
    MdPlotData *outputData;
    MdPlotData *throttleData;
    MdPlotData *aggData;

    QHBoxLayout *myhorizontalLayout;
};

#endif /* BOOSTPLOT_H_ */
