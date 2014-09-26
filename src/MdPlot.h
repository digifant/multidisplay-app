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

#ifndef MDPLOT_H_
#define MDPLOT_H_


#include "MdData.h"

#include <QWidget>
#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QHBoxLayout>
#include <qwt_plot.h>

#include "MdPlotData.h"

#include <visconfigdialog.h>

class MdPlotData;
class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotCurve;
class MdPlotZoomer;
class MdPlotPicker;
class QEvent;
class QGestureEvent;


/**
 * \brief abstract base class for our plots
 */
class MdPlot  : public QwtPlot {
    Q_OBJECT

public:
    MdPlot( QMainWindow* mw, QWidget* parent, QTableView *tableView=0 );
    virtual ~MdPlot();

    virtual void addRecord ( MdSensorRecord* r, bool doReplot=true ) = 0;
    virtual void clear () = 0;

    virtual void setWinSize (const int &nws);
    virtual void setWinMark (const int &nwm, const int &maxMark=100);
    virtual void setWinMarkMicroRelative (const int &quotient, const bool &left, const int &maxMark);

    //! our autoscaler
    virtual void setXBottomScaling ();

    void enableZoomMode(bool on);
    void toggleZoomMode();

    virtual void writeSettings ();
    virtual void readSettings ();

    //! free mem
    iResultList* getCurveValuesForXValue (const QPointF &pos);
    QColor getCurveColor (int curvenum);

public slots:
    virtual void enableXBottomAutoScale();
    virtual void pointSelected(QPointF const&);
    virtual void showCfgDialog();
    virtual void acceptCfgDialog();
    virtual void rejectCfgDialog();
    virtual void replot();

protected:
    bool event ( QEvent * event );
    bool gestureEvent(QGestureEvent *event);

    //! curves in this map get configured by the config dialog
    QMap<QString, QwtPlotCurve*> curveMap;

    MdPlotZoomer *d_zoomer[2];
    MdPlotPicker *d_picker[2];
    QwtPlotPanner *d_panner;

    QString plotnameInSavedSettings;

    QTableView* tableView;

private:
    QMainWindow* mainWindow;
    QHBoxLayout *myhorizontalLayout;
    VisConfigDialog *myCfgDialog;
};


#endif /* MDPLOT_H_ */
