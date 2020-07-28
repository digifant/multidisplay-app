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

#ifndef VISUALIZATIONPLOT_H_
#define VISUALIZATIONPLOT_H_

#include "MdPlot.h"
#include "MdPlotData.h"

#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>


class VisualizationPlot : public MdPlot {
	Q_OBJECT

public:
    VisualizationPlot( QMainWindow* mw, QWidget *parent, QTableView *tableView=0 );
	virtual ~VisualizationPlot();

    void addRecord ( MdSensorRecord* r, bool doReplot=true );
	void clear ();

    int windowBegin();

public slots:
    virtual void pointSelected(QPointF const&);
	virtual void enableXBottomAutoScale();

    //qwt5
	void showCurve(QwtPlotItem *item, bool on);
    //qwt6
    void showCurve(QVariant info, bool on, int index);
    void showCurve(QVariant info, int index);

    void removeLastMarker ();

private:
	Ui::MultidisplayUIMainWindowClass *ui;
	QwtPlotCurve *boostCurve;
	QwtPlotCurve *rpmCurve;
	QwtPlotCurve *lambdaCurve;
	QwtPlotCurve *throttleCurve;
	QwtPlotCurve *egt0Curve;
	QwtPlotCurve *egt1Curve;
    QwtPlotCurve *egt2Curve;
    QwtPlotCurve *egt3Curve;
    QwtPlotCurve *egt4Curve;
    QwtPlotCurve *egt5Curve;
//	QwtPlotCurve *egt6Curve;
//	QwtPlotCurve *egt7Curve;
	QwtPlotCurve *VDOPres1Curve;
	QwtPlotCurve *VDOPres2Curve;
	QwtPlotCurve *VDOPres3Curve;
	QwtPlotCurve *VDOTemp1Curve;
	QwtPlotCurve *VDOTemp2Curve;
	QwtPlotCurve *VDOTemp3Curve;
    QwtPlotCurve *lmmCurve;
    QwtPlotCurve *speedCurve;
    QwtPlotCurve *gearCurve;
    QwtPlotCurve *n75Curve;
    QwtPlotCurve *dfRetardCurve;
    QwtPlotCurve *dfKnockCurve;
    QwtPlotCurve *dfIgnCurve;

	MdPlotData *boostData;
	MdPlotData *rpmData;
	MdPlotData *lambdaData;
	MdPlotData *throttleData;
	MdPlotData *egt0Data;
	MdPlotData *egt1Data;
    MdPlotData *egt2Data;
    MdPlotData *egt3Data;
    MdPlotData *egt4Data;
    MdPlotData *egt5Data;
//        MdPlotData *egt6Data;
//        MdPlotData *egt7Data;
	MdPlotData *VDOPres1Data;
	MdPlotData *VDOPres2Data;
	MdPlotData *VDOPres3Data;
	MdPlotData *VDOTemp1Data;
	MdPlotData *VDOTemp2Data;
	MdPlotData *VDOTemp3Data;
    MdPlotData *lmmData;
    MdPlotData *speedData;
    MdPlotData *gearData;
    MdPlotData *n75Data;
    MdPlotData *dfRetardData;
    MdPlotData *dfKnockData;
    MdPlotData *dfIgnData;

	QList<QwtPlotMarker*> markerList;
    QList<int> markerMillisecsList;
};
#endif /* VISUALIZATIONPLOT_H_ */
