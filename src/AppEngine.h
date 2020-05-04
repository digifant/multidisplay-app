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


#ifndef APPENGINE_H
#define APPENGINE_H


#include <qglobal.h>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QMainWindow>
#else
    #include <QtGui/QMainWindow>
#endif

#include <QPointer>

#include "com/MdAbstractCom.h"
#include "com/MdBinaryProtocol.h"
#include "TransferFunction.h"
#include "serialoptions.h"

#define MDMODE true

class MdBluetoothCom;
class MdBinaryProtocol;
class MdData;
class CarPcDemoWidget;
class RealTimeVis;
class EvaluationWindow;
class MultidisplayUIMainWindow;
class MobileMainWindow;
class AndroidMainWindow;
class AndroidDashboardDialog;
class MobileBoostPidWindow;
class MobileVis1MainWindow;
class QTableWidget;
class MdBoostPidRecord;
class MobileCommandWindow;
class QSpinBox;
class QSlider;
class QTableWidget;
class ReplayWorker;
class JobRunnerThread;
class N75OptionsDialog;
class V2N75SetupDialog;
class N75PidSettingsWidget;
class V2SettingsDialog;
class Map16x1;
class MobileEvaluationDialog;
class GearSettingsDialog;
class AboutDialog;
class DigifantApplicationWindow;
class MobileGPS;
class Accelerometer;

/*
 \brief this class encapsulates the app logic for both desktop and mobile versions
 */
class AppEngine : public QObject {
    Q_OBJECT

public:
    static AppEngine* getInstance();

    SerialOptionsDialog* serialOptionsDialog();

    void show();

    double getReplaySpeedUpFactor()  { return replaySpeedUpFactor; }
    int getReplayStartAtPos() { return replayStartAtPos; }
    MdData* getData () { return this->data; }

    bool getActualizeVis1() { return actualizeVis1; }
    void setActualizeVis1( bool n ) { actualizeVis1 = n; }
    bool getActualizeDashboard() { return actualizeDashboard; }
    void setActualizeDashboard( bool n ) { actualizeDashboard = n; }

    MdBinaryProtocol* getMdBinaryProtocl() { return mds; }
    MdAbstractCom* getMdCom() { return mdcom; }

    EvaluationWindow* getEvalWinBoostLambda () { return evalWinBoostLambda; }
    EvaluationWindow* getEvalWinRPMBoost() { return evalWinRPMBoost; }
    EvaluationWindow* getEvalWinBoostLambdaSpectro() { return evalWinBoostLambdaSpectro; }

    //! can be NULL on PC
    MobileGPS *getGps() { return mGps; };
    //! can be NULL on PC
    Accelerometer *getAccelerometer () { return accelMeter; };

    TransferFunction* getDfBoostTransferFunction() { return dfBoostTransferFunction; }
    void setDfBoostTransferFunction( TransferFunction* t ) { delete (dfBoostTransferFunction); dfBoostTransferFunction=t; emit newDfBoostTransferFunction (dfBoostTransferFunction->name()); }

    TransferFunction* getWbLamdaTransferFunction() { return wbLambdaTransferFunction; }
    void setWbLambdaTransferFunction( TransferFunction* t ) { delete (wbLambdaTransferFunction); wbLambdaTransferFunction=t; }

    quint8 numConnectedTypeK;

signals:
    void showStatusMessage (QString);
    void showStatusBarSampleCount (QString);
    void newDfBoostTransferFunction (int);

public slots:
    void setReplaySpeedUpFactor(double f)  { replaySpeedUpFactor = f; }


    void saveData ();
    void saveDataAs ();
    void saveDataAsCSV();
    void openData ( QString fn="" );
    void clearData ();

    void changeSerialOptions();
    //! compute and set the window mark that the given record number gets displayed
    void changeDataWinMarkToDisplayRecord(const int record);
    void changeDataWinMark(const int &nwm);
    void changeDataWinSize (int ns);
    void changeDVSliderUp();
    void changeDVSliderDown();
    void changeDVSliderMin();
    void changeDVSliderMax();

    void changeDataWinMarkMicroLeft ();
    void changeDataWinMarkMicroRight ();

    void writeSettings ();

    void replayData();

    //! used by AndroidMainWindows eventhandler to re-create the dialog after destroy on close
    void reCreateDialogsAndroidFix();

protected:
    void closeEvent ( QCloseEvent * event );

private:
    AppEngine();
    ~AppEngine();

    void setupPC();
    void setupMobile();
    void setupMaemo();
    void setupAndroid();

    void readSettings ();

    SerialOptionsDialog *mySerialOptionsDialog;

    EvaluationWindow *evalWinBoostLambda;
    EvaluationWindow *evalWinRPMBoost;
    EvaluationWindow *evalWinBoostLambdaSpectro;
    N75OptionsDialog *n75OptionsDialog;
    QPointer<V2N75SetupDialog> v2N75SetupDialog;
    QPointer<N75PidSettingsWidget> n75PidSettingsOnBoostTab;
    QPointer<V2SettingsDialog> v2SettingsDialog;
    QPointer<GearSettingsDialog> gearSettingsDialog;
    QPointer<AboutDialog> aboutDialog;

    QPointer<MdBinaryProtocol> mds;
    QPointer <MdAbstractCom> mdcom;

    MdData *data;

    QString filename;
    CarPcDemoWidget *demo1;
    RealTimeVis *rtvis;

    //PC
    MultidisplayUIMainWindow *pcmw;
    //Maemo
    MobileMainWindow *mmw;
    //Android
    AndroidMainWindow *amw;
    AndroidDashboardDialog* add;

    MobileBoostPidWindow *mbw;
    MobileVis1MainWindow *mvis1w;
    MobileCommandWindow *mcw;
    MobileEvaluationDialog* mevalDialog;
    MobileGPS *mGps;
    Accelerometer *accelMeter;

    QSlider *DataViewSlider;
    QSpinBox *DataViewSpinBox;
    QTableWidget *BoostPIDTableWidget;

    //Replay
    JobRunnerThread *replayThread;
    ReplayWorker *replay;

    double replaySpeedUpFactor;
    int replayStartAtPos;

    //settings
    bool actualizeVis1;
    bool actualizeDashboard;
    bool dashboardActualizeSave;
    bool vis1ActualizeSave;
    TransferFunction* dfBoostTransferFunction;
    TransferFunction* wbLambdaTransferFunction;
    DigifantApplicationWindow* dfAppWin;

    QString directory;
};

#endif // APPENGINE_H
