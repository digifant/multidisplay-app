/*
    Copyright 2010-2012 Stephan Martin, Dominik Gummel

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

#ifndef MDDATA_H_
#define MDDATA_H_

#define MAX_ATTACHED_TYPK 8

#include "ui_multidisplayuimainwindow.h"
#include "ColorOverBlend.h"
#include "Map16x1.h"
#include "DataTableConfigDialog.h"
#include "mobile/MobileSensorRecord.h"

#include <list>

#include <QList>
#include <QLinkedList>
#include <QVector>
#include <QString>
#include <QDataStream>
#include <QObject>
#include <QString>
#include <QAbstractTableModel>
#include <QSplitter>
#include <QMenu>
#include <QAction>

#define MAXVALUES 9
#define MAXVAL_BOOST 0
#define MAXVAL_LMM 1
#define MAXVAL_LAMBDA 2
#define MAXVAL_RPM 3
#define MAXVAL_SPEED 4
#define MAXVAL_OILPRES 5
#define MAXVAL_GASPRES 6
#define MAXVAL_EGT 7
#define MAXVAL_EFR_SPEED 8

class QSplashScreen;
class QProgressBar;
class QLabel;
class QwtPlot;
class BoostPidPlot;
class VisualizationPlot;
class MdPlot;
class QTimer;
class MaxDataSet;
class V2PowerDialog;
class WotEventsDialog;


/**
  * TODO:
  * add mapped battery voltage
  * add external knock
  * add reserved bytes
  */
class MdSensorRecord {
    friend QDataStream& operator<< (QDataStream& s, MdSensorRecord *r);
    friend QDataStream& operator>> (QDataStream& s, MdSensorRecord *r);


public:
    MdSensorRecord ();
    MdSensorRecord ( int time, int rpm, int throttle, double boost, double lambda, double lmm,
                     double casetemp, double agt0, double agt1, double agt2, double agt3, double agt4,
                     double agt5, double agt6, double agt7, double batcur,
                     double VDOPres1, double VDOPres2, double VDOPres3,
                     double VDOTemp1, double VDOTemp2, double VDOTemp3,
                     double speed, int gear, int n75, double n75_req_boost, quint8 n75_req_boost_pwm, quint8 flags,
                     double efr_speed,
                     quint8 df_boost_raw, quint8 df_lambda, quint8 df_knock_raw, quint8 df_ect_raw, quint8 df_iat_raw,
                     quint8 df_co_poti, quint8 df_flags, quint8 df_ign,
                     quint8 df_cyl1_knock_retard, quint8 df_cyl1_knock_decay, quint8 df_cyl2_knock_retard,
                     quint8 df_cyl2_knock_decay, quint8 df_cyl3_knock_retard, quint8 df_cyl3_knock_decay, quint8 df_cyl4_knock_retard,
                     quint8 df_cyl4_knock_decay, quint8 df_voltage_raw, quint16 df_inj_time,
                     quint8 df_cold_startup_enrichment, quint8 df_warm_startup_enrichment, quint8 df_ect_enrichment,
                     quint8 df_acceleration_enrichment, quint8 df_counter_startup_enrichment, quint8 df_iat_enrichment,
                     quint8 df_ignition_addon_counter, quint8 df_igniton_addon, quint8 df_ect_injection_addon, quint16 df_rpm_delta_hall,
                     quint8 df_isv, quint8 df_lc_flags,
                     double df_ignition_total_retard, double df_ect, double df_iat, double df_ignition, double df_voltage,
                     quint16 knock, quint16 df_kline_freq, quint8 df_kline_framenum
                     );
    MdSensorRecord ( const MdSensorRecord& d );
    virtual ~MdSensorRecord();

    double getVDOTemp1() const;
    double getVDOTemp2() const;
    double getVDOTemp3() const;
    double getVDOPres1() const;
    double getVDOPres2() const;
    double getVDOPres3() const;
    double getEgt0() const;
    double getEgt1() const;
    double getEgt2() const;
    double getEgt3() const;
    double getEgt4() const;
    double getEgt5() const;
    double getEgt6() const;
    double getEgt7() const;
    //! returns the highest egt value (key name temp) and the idx of the typK (key name idx)
    QMap<QString, double> getHighestEgt() const;
    double getBatcur() const;
    double getBoost() const;
    double getCasetemp() const;
    double getLambda() const;
    double getLmm() const;
    int getRpm() const;
    int getThrottle() const;
    int getTime() const;
    double getSpeed() const;
    int getGear() const;
    int getN75() const;
    double getN75ReqBoost();
    quint8 getN75ReqBoostPWM();
    quint8 getFlags();
    void setEgt0(double agt);
    void setEgt1(double agt);
    void setEgt2(double agt);
    void setEgt3(double agt);
    void setEgt4(double agt);
    void setEgt5(double agt);
    void setEgt6(double agt);
    void setEgt7(double agt);
    void setBatcur(double batcur);
    void setBoost(double boost);
    void setCasetemp(double casetemp);
    void setLambda(double lambda);
    void setLmm(double lmm);
    void setRpm(int rpm);
    void setThrottle(int throttle);
    void setTime(int time);
    void setSpeed (int speed);
    void setGear (int g);
    void setN75 (int n);
    void setN75ReqBoost (double b);
    void setN75ReqBoostPWM (quint8 p);
    void setFlags (quint8 f);

    QVariant getColumn ( const int & column );

protected:
    int time;
    int rpm;
    int throttle;
    double boost;
    double lambda;
    double lmm;
    double casetemp;
    double egt[MAX_ATTACHED_TYPK];
    double batcur;
    double VDOPres1;
    double VDOPres2;
    double VDOPres3;
    double VDOTemp1;
    double VDOTemp2;
    double VDOTemp3;
    double speed;
    int gear;
    int n75;
    double n75_req_boost;
    quint8 n75_req_boost_pwm;
    quint8 flags;

public:
    double efr_speed;
    quint16 df_kline_freq;
    quint8 df_kline_framenum;

    //HACK: make it public to save time
    //ECU raw Data from Digifant I
    quint8 df_boost_raw;
    quint8 df_lambda;
    quint8 df_knock_raw;
    quint8 df_ect_raw;
    quint8 df_iat_raw;
    quint8 df_co_poti;
    quint8 df_flags;
    quint8 df_ign;
    quint8 df_cyl1_knock_retard;
    quint8 df_cyl1_knock_decay;
    quint8 df_cyl2_knock_retard;
    quint8 df_cyl2_knock_decay;
    quint8 df_cyl3_knock_retard;
    quint8 df_cyl3_knock_decay;
    quint8 df_cyl4_knock_retard;
    quint8 df_cyl4_knock_decay;
    quint8 df_voltage_raw;
    quint16 df_inj_time;
    quint8 df_cold_startup_enrichment;
    quint8 df_warm_startup_enrichment;
    quint8 df_ect_enrichment;
    quint8 df_acceleration_enrichment;
    quint8 df_counter_startup_enrichment;
    quint8 df_iat_enrichment;
    quint8 df_ignition_addon_counter;
    quint8 df_igniton_addon;
    quint8 df_ect_injection_addon;
    quint16 df_rpm_delta_hall;
    quint8 df_isv;
    quint8 df_lc_flags;

    //mapped DF1 data
    double df_ignition_total_retard;
    double df_ect;
    double df_iat;
    double df_ignition;
    double df_voltage;
    double df_inj_duty;

    //additional for phormula knock tool
    quint16 knock;
};

QDataStream& operator<< (QDataStream& s, MdSensorRecord *r);
QDataStream& operator>> (QDataStream& s, MdSensorRecord *r);


class MdDataRecord {
    friend QDataStream& operator<< (QDataStream& s, MdDataRecord *d);
    friend QDataStream& operator>> (QDataStream& s, MdDataRecord *d);

public:
    MdDataRecord ( );
    MdDataRecord ( MdSensorRecord *sr );
    virtual ~MdDataRecord ( );

    MobileSensorRecord *getMobileR() const { return mobileR; };
    void setMobileR(MobileSensorRecord *mobileR) { if (mobileR) delete (mobileR); this->mobileR = mobileR; };

    MdSensorRecord *getSensorR() const;
    void setSensorR(MdSensorRecord *sensorR);

    QVariant getColumn ( const int & column );

protected:
    MdSensorRecord *sensorR;
    MobileSensorRecord *mobileR;
};
QDataStream& operator<< (QDataStream& s, MdDataRecord *d);
QDataStream& operator>> (QDataStream& s, MdDataRecord *d);


//class RealTimeVis;

class MdData : public QAbstractTableModel {
    Q_OBJECT

public:
    MdData (QMainWindow* mw_boost, QWidget* parent_boost, QMainWindow* mw_vis1, QWidget* parent_vis1,
            QTableView* dataView=NULL);
    virtual ~MdData();

    void addDataRecord (MdDataRecord* nr, bool doReplot=true);
    void checkMaxValues (MdDataRecord* nr);

    //! attention, sensor or pid object can be NULL!
    QList<MdDataRecord*>& getData ();

    int size();

    void saveData ();
    bool saveDataCSV ( const QString& filename );
    bool saveData ( const QString& filename, int begin=0, int end=0 );
    virtual bool loadData ( const QString& filename );

    virtual void writeSettings ();
    virtual void readSettings ();


    void clearData ();
    void replot();

    VisualizationPlot* getVisPlot();

    // 2010-07-20: file storage version 2 supports vdo
    // 2011-07-13: new binary protocol for MD version 2 (version 3)
    // 2013-04-23: new binary protocol for MD version 2 (version 4) including efr speed
    enum { MAGICNUMBER = 0xFFAAFFAA, VERSION4 = 4, CVERSION3 = 3, CVERSION1 = 1 };

    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    bool insertRows(int position, int rows, const QModelIndex &parent);
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );

//    void setRTVis (RealTimeVis* vis) { rtvis = vis; };

    QModelIndex findRowForMillis ( quint32 millis );

    //! returns the time index of the newest data record
    int getLastTime ();

    void findWot ();
    QList<int> findKnock ( bool showWindow=true );
    QList<int> findHighEGT ( bool showWindow=true);
    QList<int> findInjectorHighDC ( bool showWindow=true);
    void checkData ();



signals:
    void showStatusMessage (QString);
    void rtNewDataRecord (MdDataRecord *nr);
    //! table rows!
    void showRecordInVis1 (int record);

public slots:
    //! set the window mark to display this record set
    int changeDataWinMarkToDisplayRecord (const int &element, const int &maxMark);
    void changeDataWinMarkMicroRelative (const int &quotient, const bool &left, const int &maxMark);
    void changeDataWinMark (const int &nm, const int &maxMark=100);
    void changeDataWinSize (const int &ns);
    void toggleZoomMode();

    void showCfgVis1 ();
    void showCfgDataTable();
    void acceptCfgDataTable();

    void clearPlots();
    void visualizeDataRecord (MdDataRecord* nr, bool doReplot=true);

    //! helper for operations on selected cells; list is not sorted!
    QList<int> helperGetUniqueRows (QItemSelectionModel *select );
    void tableDataView_customContextMenu( const QPoint& );

    void showDataListIdx (int);

private:

    // make changes thread safe!
    QList<MdDataRecord*> dataList;

    QVector<QString> headerColNames;

    QSplashScreen* splash;
    QProgressBar* progressBar;
    QLabel* splashLabel;

    QMainWindow* mainWindow;

    BoostPidPlot* boostPidPlot;
    VisualizationPlot* visPlot;
//    VisualizationPlot* visPlot2;

    V2PowerDialog* powerDialog;
    WotEventsDialog* wotEventsDialog;

    QLinkedList<MdPlot*> plotList;

    QTableView* dataView;

//    QSplitter *visTabSplitter;
//    QHBoxLayout *hbl;
//    QList<int> *sl;

//    RealTimeVis *rtvis;

    QMenu* dataViewContextMenu;
    QAction *dataViewContextMenuSaveMarkedRows;
    QAction *dataViewContextMenuDelItemAction;
    QAction *dataViewContextMenuDigifantBoost2MdBoost;
    QAction *dataViewContextMenuShowinVis1;
    QAction *dataViewContextMenuPowerPlot;
    QAction *dataViewContextMenuPowerPlotGPS;
    QAction *dataViewContextMenuCalc100to200;
    QAction *dataViewContextMenuCalc100to200GPS;
    QAction *dataViewContextMenuFindWotEvents;
    QAction *dataViewContextMenuFindKnockEvents;
    QAction *dataViewContextMenuFindHighEGTEvents;
    QAction *dataViewContextMenuCheckEvents;

    DataTableConfigDialog* dataTableConfigDialog;

    ColorOverBlend *egtBlend;
    ColorOverBlend *efrBlend;
    ColorOverBlend *knockBlend;
    ColorOverBlend *boostBlend;
    ColorOverBlend *iatBlend;
    ColorOverBlend *voltageBlend;
    ColorOverBlend *lambdaBlend;
    ColorOverBlend *rawKnockBlend;
    ColorOverBlend *injectorDutyBlend;

    Map16x1_ISV *isvMap;

    MaxDataSet* maxValues[MAXVALUES];
};




class MaxDataSet {
public:
    MaxDataSet ();
    MdSensorRecord* p;
};


#endif /* MDDATA_H_ */
