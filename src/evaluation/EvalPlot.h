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


#ifndef EVALPLOT_H_
#define EVALPLOT_H_

#include "qwt_plot_curve.h"
#include "qwt_plot.h"

#include "MdPlot.h"
#include "EvalPlotData.h"

class MdData;

class EvalPlot: public MdPlot {
    //class EvalPlot: public QwtPlot {
public:
    EvalPlot( QMainWindow* mw, QWidget *parent );
    virtual ~EvalPlot();

    virtual void compute ( MdData *md ) = 0;


    virtual void clear();
    virtual void addRecord(MdSensorRecord* r, bool doReplot=true);

public slots:
    virtual void replot();

protected:
    EvalPlotDataSimple d;

    QwtPlotCurve *ec;
};


class EvalPlotBoostLambda : public EvalPlot {
public:
    EvalPlotBoostLambda ( QMainWindow* mw, QWidget *parent );
    virtual ~EvalPlotBoostLambda();

    void compute ( MdData *md );
};


class EvalPlotRPMBoost : public EvalPlot {
public:
    EvalPlotRPMBoost ( QMainWindow* mw, QWidget *parent );
    virtual ~EvalPlotRPMBoost();

    void compute ( MdData *md );
};


#endif /* EVALPLOT_H_ */
