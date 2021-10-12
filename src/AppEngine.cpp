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

#include "AppEngine.h"


#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <QApplication>
#include <QTableWidget>
#include <QDesktopServices>
#include <QDir>

#include "AppEngine.h"
#include "MdData.h"
#include "widgets/realtimevis.h"
#include "VisualizationPlot.h"
#include "evaluation/evaluationwindow.h"
#include "multidisplayuimainwindow.h"
#include "mobile/mobilemainwindow.h"
#include "mobile/MobileBoostPidWindow.h"
#include "mobile/MobileVis1MainWindow.h"
#include "mobile/MobileCommandWindow.h"
#include "mobile/AndroidMainWindow.h"
#include "mobile/AndroidDashboardDialog.h"
#include "ui_AndroidMainWindow.h"
#include "thread/jobrunnerthread.h"
#include "thread/replayworker.h"
#include "Map16x1.h"
#include "N75OptionsDialog.h"
#include "ui_N75OptionsDialog.h"
#include "V2N75SetupDialog.h"
#include "V2SettingsDialog.h"
#include "mobile/MobileEvaluationDialog.h"
#include "GearSettingsDialog.h"
#include "AboutDialog.h"
#include "TransferFunction.h"
#include "DigifantApplicationWindow.h"

#include "ui_mobilemainwindow.h"
#include "ui_MobileBoostPidWindow.h"
#include "ui_MobileVis1MainWindow.h"
#include "ui_V2N75SetupDialog.h"
#include "widgets/MyTableWidget.h"
#include "widgets/N75PidSettingsWidget.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include "com/MdQSerialPortCom.h"
#endif

#include "com/MdBluetoothCom.h"
#include "com/MdBluetoothLECom.h"
#include "com/MdBluetoothWrapper.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    #include "com/MdQextSerialCom.h"
#endif

#if defined (Q_OS_ANDROID)
    #include "com/MdBluetoothCom.h"
    #include "com/MdBluetoothLECom.h"
    #include <QtAndroid>
#endif

#if defined (Q_OS_IOS)
    #include "com/MdBluetoothCom.h"
    #include "com/MdBluetoothLECom.h"
    #include "mobile/MobileGPS.h"
    #include "mobile/Accelerometer.h"
#endif

#if defined (Q_WS_MAEMO_5) || defined(ANDROID)
#include "mobile/MobileGPS.h"
#include "mobile/Accelerometer.h"
#include "mobile/AndroidN75Dialog.h"
#endif

AppEngine* AppEngine::getInstance() {
    static AppEngine instance;
    return &instance;
}

AppEngine::AppEngine() {

#if  !defined (Q_WS_MAEMO_5)  && !defined (ANDROID) && !defined (Q_OS_IOS)
    qDebug() << "desktop version";
    pcmw = new MultidisplayUIMainWindow ();
    mmw = nullptr;
    mbw = nullptr;
    mvis1w = nullptr;
    mcw = nullptr;
    amw = nullptr;

    wbLambdaTransferFunction = nullptr;

    DataViewSlider = pcmw->ui.DataViewSlider;
    DataViewSpinBox = pcmw->ui.DataViewWinSizeSpinBox;

    data = new MdData(pcmw, pcmw->ui.BoostGraphGroupBox, pcmw, pcmw->ui.VisualizationTab, pcmw->ui.DataTableView );

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    mdcom = new MdQSerialPortCom(this);
    //mdcom = new MdBluetoothLECom(this);
    //mdcom = new MdBluetoothWrapper(this);
#else
    mdcom = new MdQextSerialCom(this);
#endif
    mds = new MdBinaryProtocol(this, data, mdcom);

    mySerialOptionsDialog = new SerialOptionsDialog ();

    evalWinBoostLambda = new EvaluationWindow (NULL, data, EvaluationWindow::BoostLambda );
    evalWinRPMBoost = new EvaluationWindow (NULL, data, EvaluationWindow::RPMBoost );
    evalWinBoostLambdaSpectro = new EvaluationWindow (NULL, data, EvaluationWindow::SpectroBoostLambda );
    n75OptionsDialog = new N75OptionsDialog(pcmw);
    v2N75SetupDialog = new V2N75SetupDialog(pcmw);
    v2SettingsDialog = new V2SettingsDialog(pcmw);
    gearSettingsDialog = new GearSettingsDialog(pcmw);
    aboutDialog = new AboutDialog(pcmw);
    dfAppWin = new DigifantApplicationWindow (pcmw);

#endif

    actualizeVis1 = true;
    actualizeDashboard = true;

#if  defined (Q_WS_MAEMO_5)
    //http://doc.trolltech.com/qt-maemo-4.6/platform-notes-maemo5.html

    qDebug() << "QT_WS_MAEMO_5 mobile version";
    QDesktopWidget *dw = QApplication::desktop ();
    mmw = new MobileMainWindow ();
//    mmw->setGeometry( dw->availableGeometry() );
//    mmw->showExpanded();
    pcmw = NULL;
    amw = NULL;

    //Maemo 5 Stacked Windows are support by setting the Qt::WA_Maemo5StackedWindow widget attribute on all top-level windows that should be part of the stack.
    //The root window should not have a parent, while all other windows in the stack should have their predecessor in the stack set as their parent
    //(do not forget to use the Qt::Window widget flag, if you are not using QMainWindow derived classes).

    //FIXME
    mbw = new MobileBoostPidWindow (mmw);
    mbw->setGeometry( dw->availableGeometry() );
    mvis1w = new MobileVis1MainWindow (mbw);

    DataViewSlider = mvis1w->ui->DVSlider;
    DataViewSpinBox = mvis1w->ui->DVSpinBox;

    data = new MdData(mbw, mbw->ui->BoostGraphGroupBox, mvis1w, mvis1w->ui->Vis1PlotBox );
    mdcom = new MdQextSerialCom(this);
    mds = new MdBinaryProtocol(this, data, mdcom);
    mySerialOptionsDialog = new SerialOptionsDialog ();
    mySerialOptionsDialog->setGeometry( dw->availableGeometry() );
    mySerialOptionsDialog->setGeometry( QRect(0,0,700,350) );
    mcw = new MobileCommandWindow(mmw);
    aboutDialog = new AboutDialog(mmw);

    evalWinBoostLambda = new EvaluationWindow (mmw, data, EvaluationWindow::BoostLambda );
    Q_ASSERT(evalWinBoostLambda != NULL);
    evalWinRPMBoost = new EvaluationWindow (mmw, data, EvaluationWindow::RPMBoost );
    Q_ASSERT(evalWinRPMBoost != NULL);
    //	evalWinBoostLambdaSpectro = new EvaluationWindow (NULL, data, EvaluationWindow::SpectroBoostLambda );

#ifndef ANDROID
    mevalDialog = new MobileEvaluationDialog (mmw, evalWinBoostLambda, evalWinRPMBoost);
#endif
    v2N75SetupDialog = new V2N75SetupDialog(mmw);
    v2SettingsDialog = new V2SettingsDialog(mmw);
    gearSettingsDialog = new GearSettingsDialog(mmw);
    //FIXME remove it
    BoostPIDTableWidget = mbw->ui->BoostPIDTableWidget;
#endif


#if defined (Q_OS_ANDROID)
    qDebug() << "ANDROID mobile version";

#if QT_VERSION > QT_VERSION_CHECK(5, 10, 0)
    auto  result = QtAndroid::checkPermission(QString("android.permission.READ_EXTERNAL_STORAGE"));
    if(result == QtAndroid::PermissionResult::Denied){
        QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({"android.permission.READ_EXTERNAL_STORAGE"}));
        if(resultHash["android.permission.READ_EXTERNAL_STORAGE"] == QtAndroid::PermissionResult::Denied)
            qDebug() << "READ_EXTERNAL_STORAGE permission denied!";
    }
    result = QtAndroid::checkPermission(QString("android.permission.WRITE_EXTERNAL_STORAGE"));
    if(result == QtAndroid::PermissionResult::Denied){
        QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({"android.permission.WRITE_EXTERNAL_STORAGE"}));
        if(resultHash["android.permission.WRITE_EXTERNAL_STORAGE"] == QtAndroid::PermissionResult::Denied)
            qDebug() << "WRITE_EXTERNAL_STORAGE permission denied!";
    }
    result = QtAndroid::checkPermission(QString("android.permission.BLUETOOTH"));
    if(result == QtAndroid::PermissionResult::Denied){
        QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({"android.permission.BLUETOOTH"}));
        if(resultHash["android.permission.BLUETOOTH"] == QtAndroid::PermissionResult::Denied)
            qDebug() << "BLUETOOTH permission denied!";
    }
    result = QtAndroid::checkPermission(QString("android.permission.BLUETOOTH_ADMIN"));
    if(result == QtAndroid::PermissionResult::Denied){
        QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({"android.permission.BLUETOOTH_ADMIN"}));
        if(resultHash["android.permission.BLUETOOTH_ADMIN"] == QtAndroid::PermissionResult::Denied)
            qDebug() << "BLUETOOTH_ADMIN permission denied!";
    }
    result = QtAndroid::checkPermission(QString("android.permission.ACCESS_FINE_LOCATION"));
    if(result == QtAndroid::PermissionResult::Denied){
        QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({"android.permission.ACCESS_FINE_LOCATION"}));
        if(resultHash["android.permission.ACCESS_FINE_LOCATION"] == QtAndroid::PermissionResult::Denied)
            qDebug() << "ACCESS_FINE_LOCATION permission denied!";
    }
#endif

    amw = new AndroidMainWindow ();

    pcmw = NULL;
    mmw = NULL;
    //FIXME
    mbw = new MobileBoostPidWindow (amw);
    mvis1w = new MobileVis1MainWindow (mbw);

    data = new MdData(mbw, mbw->ui->BoostGraphGroupBox, mvis1w, mvis1w->ui->Vis1PlotBox );

    //mdcom = new MdBluetoothCom(this);
    mdcom = new MdBluetoothWrapper(this);
    mds = new MdBinaryProtocol(this, data, mdcom);

    mySerialOptionsDialog = new SerialOptionsDialog ();
    //mySerialOptionsDialog->setGeometry( QRect(0,0,700,350) );
    mcw = new MobileCommandWindow(amw);
    aboutDialog = new AboutDialog(amw);

    evalWinBoostLambda = new EvaluationWindow (amw, data, EvaluationWindow::BoostLambda );
    Q_ASSERT(evalWinBoostLambda != NULL);
    evalWinRPMBoost = new EvaluationWindow (amw, data, EvaluationWindow::RPMBoost );
    Q_ASSERT(evalWinRPMBoost != NULL);
    //	evalWinBoostLambdaSpectro = new EvaluationWindow (NULL, data, EvaluationWindow::SpectroBoostLambda );

    v2N75SetupDialog = new V2N75SetupDialog(amw);
    v2SettingsDialog = new V2SettingsDialog(amw);
    gearSettingsDialog = new GearSettingsDialog(amw);

    actualizeVis1 = false;
    actualizeDashboard = false;
#endif

#if defined (Q_OS_IOS)
    amw = new AndroidMainWindow ();

    pcmw = NULL;
    mmw = NULL;
    //FIXME
    mbw = new MobileBoostPidWindow (amw);
    mvis1w = new MobileVis1MainWindow (mbw);

    data = new MdData(mbw, mbw->ui->BoostGraphGroupBox, mvis1w, mvis1w->ui->Vis1PlotBox );

    //mdcom = new MdBluetoothCom(this);
    mdcom = new MdBluetoothLECom(this);
    //mdcom = new MdBluetoothWrapper(this);
    mds = new MdBinaryProtocol(this, data, mdcom);

    mySerialOptionsDialog = new SerialOptionsDialog ();
    //mySerialOptionsDialog->setGeometry( QRect(0,0,700,350) );
    mcw = new MobileCommandWindow(amw);
    aboutDialog = new AboutDialog(amw);

    evalWinBoostLambda = new EvaluationWindow (amw, data, EvaluationWindow::BoostLambda );
    Q_ASSERT(evalWinBoostLambda != NULL);
    evalWinRPMBoost = new EvaluationWindow (amw, data, EvaluationWindow::RPMBoost );
    Q_ASSERT(evalWinRPMBoost != NULL);
    //	evalWinBoostLambdaSpectro = new EvaluationWindow (NULL, data, EvaluationWindow::SpectroBoostLambda );

    v2N75SetupDialog = new V2N75SetupDialog(amw);
    v2SettingsDialog = new V2SettingsDialog(amw);
    gearSettingsDialog = new GearSettingsDialog(amw);

    actualizeVis1 = false;
    actualizeDashboard = false;

    //TODO do we have to request rights on IOS???
#endif

    //Replay
    replaySpeedUpFactor = 1;
    replay = new ReplayWorker(this);
    replayThread = new JobRunnerThread (this, replay);

    connect (replay, SIGNAL(clearPlots()), data, SLOT(clearPlots()) );

#if  !defined (Q_WS_MAEMO_5)  && !defined (Q_OS_ANDROID)  && !defined (Q_OS_IOS)
    connect (replay, SIGNAL(showStatusMessage(QString)), pcmw, SLOT(showStatusMessage(QString)), Qt::QueuedConnection );
#endif
    connect (replay, SIGNAL(visualizeDataRecord(MdDataRecord*,bool)), data, SLOT(visualizeDataRecord(MdDataRecord*,bool)), Qt::QueuedConnection );


#if  defined (Q_WS_MAEMO_5)  || defined (Q_OS_ANDROID) || defined (Q_OS_IOS)
    setupMobile();
#else
    setupPC();
#endif
    readSettings();

#if QT_VERSION >= 0x050000
    directory = QStandardPaths::standardLocations (QStandardPaths::DocumentsLocation)[0];
#else
    directory = QDesktopServices::storageLocation (QDesktopServices::DocumentsLocation);
#endif
#if defined Q_OS_ANDROID
    //QAndroidJniObject s = QAndroidJniObject::callStaticObjectMethod( "de/gummelinformatics/mui/MuiIntentHelper", "getPublicDocumentPath", "()Ljava/lang/String;" );
    QAndroidJniObject s = QAndroidJniObject::callStaticObjectMethod( "de/gummelinformatics/mui/MuiIntentHelper", "getPublicDownloadsPath", "()Ljava/lang/String;" );
    directory = s.toString();
    qDebug() << "android path 2020: " << directory;
#endif

    //load TEST-DATA
//    openData ("/home/bofh/dev/multidisplay/2/log/incoming/2014-05-21T2252-977Grad_AGT.mdv2");

}


AppEngine::~AppEngine() {
    //TODO
}


void AppEngine::setupPC() {
    connect (pcmw, SIGNAL(writeSettings()), this, SLOT(writeSettings()));

    connect (pcmw->ui.ButtonAHold, SIGNAL(clicked()), mds, SLOT(mdCmdAh() ) );
    connect (pcmw->ui.ButtonAPush, SIGNAL(clicked()), mds, SLOT(mdCmdAp() ) );
    connect (pcmw->ui.ButtonBHold, SIGNAL(clicked()), mds, SLOT(mdCmdBh() ) );
    connect (pcmw->ui.ButtonBPush, SIGNAL(clicked()), mds, SLOT(mdCmdBp() ) );
    connect (pcmw->ui.action_activate_MD_string_output, SIGNAL(triggered()), mds, SLOT(mdCmdActivateSerialOutput() ) );
    connect (pcmw->ui.action_disable_measurement_data_output, SIGNAL(triggered()), mds, SLOT(mdCmdDisableSerialOutput()) );
    connect (pcmw->ui.actionActivate_MD_binary_output, SIGNAL(triggered()), mds, SLOT(mdCmdActivateSerialBinaryOutput()) );
    connect (pcmw->ui.action_SerialConnect, SIGNAL(triggered()), mds, SLOT(openPort() ) );
    connect (pcmw->ui.action_SerialDisconnect, SIGNAL(triggered()), mds, SLOT(closePort() ) );
    connect (pcmw->ui.action_SerialOptions, SIGNAL(triggered()), mySerialOptionsDialog, SLOT(show()) );
    connect (mySerialOptionsDialog, SIGNAL(accepted()), this, SLOT(changeSerialOptions()) );

    connect (pcmw->ui.action_Save, SIGNAL(triggered()), this, SLOT(saveData() ) );
    connect (pcmw->ui.action_SaveAs, SIGNAL(triggered()), this, SLOT(saveDataAs() ) );
    connect (pcmw->ui.action_Open, SIGNAL(triggered()), this, SLOT(openData() ) );
    connect (pcmw->ui.action_New, SIGNAL(triggered()), this, SLOT(clearData() ) );
    connect (pcmw->ui.action_Export_as_CSV, SIGNAL(triggered()), this, SLOT(saveDataAsCSV() ) );


//    connect (pcmw->ui.action_Enable_Zoom_Mode, SIGNAL(changed()), data, SLOT (toggleZoomMode() ) );
    connect (pcmw->ui.action_config_Vis1, SIGNAL(triggered()), data, SLOT(showCfgVis1()) );
    connect (pcmw->ui.actionConfigure_DataTable, SIGNAL(triggered()), data, SLOT(showCfgDataTable()) );


    connect (pcmw->ui.actionShow_Boost_Lambda, SIGNAL(triggered()), evalWinBoostLambda, SLOT(show()) );
    connect (pcmw->ui.actionShow_RPM_Boost, SIGNAL(triggered()), evalWinRPMBoost, SLOT(show()) );
    connect (pcmw->ui.actionShow_Boost_Lambda_Spectrogram, SIGNAL(triggered()), evalWinBoostLambdaSpectro, SLOT(show()) );

    connect (pcmw->ui.actionAbout, SIGNAL(triggered()), aboutDialog, SLOT(show()));

    connect (pcmw->ui.DataViewSlider, SIGNAL(valueChanged(int)), this, SLOT(changeDataWinMark(int)));
    connect (pcmw->ui.DataViewWinSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeDataWinSize(int)));
    connect (pcmw->ui.DataViewScrollLeftButton, SIGNAL(clicked()), this, SLOT(changeDataWinMarkMicroLeft() ) );
    connect (pcmw->ui.DataViewScrollRightButton, SIGNAL(clicked()), this, SLOT(changeDataWinMarkMicroRight() ) );

    //replay
    connect (pcmw->ui.action_Replay, SIGNAL(triggered()), this, SLOT(replayData()) );
    connect (mds, SIGNAL(portOpened()), pcmw, SLOT(disableReplay()));
    connect (mds, SIGNAL(portClosed()), pcmw, SLOT(enableReplay()));
    connect (pcmw->ui.PlayButton, SIGNAL(clicked()), this, SLOT(replayData()) );
    connect (pcmw->ui.PauseButton, SIGNAL(clicked()), replay, SLOT(pause()) );
    connect (pcmw->ui.StopButton, SIGNAL(clicked()), replay, SLOT(stop()) );
    connect (pcmw->ui.ReplayFactorSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setReplaySpeedUpFactor(double)));

    connect (mdcom, SIGNAL(showStatusMessage(QString)), pcmw, SLOT(showStatusMessage(QString)) );
    connect (mds, SIGNAL(showStatusMessage(QString)), pcmw, SLOT(showStatusMessage(QString)) );
    connect (data, SIGNAL(showStatusMessage(QString)), pcmw, SLOT(showStatusMessage(QString)));
    connect (mds, SIGNAL(showStatusMessage(QString)), pcmw, SLOT(showStatusMessage(QString)));
    connect (mds, SIGNAL(showStatusBarSampleCount(QString)), pcmw, SLOT(showStatusBarSampleCount(QString)));
    connect (this, SIGNAL(showStatusMessage(QString)), pcmw, SLOT(showStatusMessage(QString)));
    connect (this, SIGNAL(showStatusBarSampleCount(QString)), pcmw, SLOT(showStatusBarSampleCount(QString)));

    pcmw->ui.DataTableView->setModel(data);
    pcmw->ui.DataTableView->setAlternatingRowColors(true);
    pcmw->ui.DataTableView->resizeRowsToContents();
    pcmw->ui.DataTableView->resizeColumnsToContents();

    //V2 config stuff
    //V2 settings
//    AndroidN75Dialog* an75 = new AndroidN75Dialog (pcmw, mds);
//    connect (pcmw->ui.actionN75_boost_control, SIGNAL(triggered()), an75, SLOT(show()));
    connect (pcmw->ui.actionN75_boost_control, SIGNAL(triggered()), v2N75SetupDialog, SLOT(show()));
    connect (pcmw->ui.actionSettings, SIGNAL(triggered()), v2SettingsDialog, SLOT(show()));
    connect (pcmw->ui.V2_action_load_settings_from_EEPROM, SIGNAL(triggered()), mds, SLOT(mdCmdLoadFromEeprom()));
    connect (pcmw->ui.V2_action_save_settings_to_EEPROM, SIGNAL(triggered()), mds, SLOT(mdCmdSave2Eeprom()));
    connect (pcmw->ui.V2_action_calibrate_LD_measure_environment_pressure, SIGNAL(triggered()), mds, SLOT(mdCmdCalBoost()));
    connect (pcmw->ui.actionGearbox_settings, SIGNAL(triggered()), gearSettingsDialog, SLOT(show()));

    connect (v2N75SetupDialog, SIGNAL(n75reqDutyMap(quint8,quint8,quint8)), mds, SLOT(mdCmdReqN75DutyMap(quint8,quint8,quint8)));
    connect (v2N75SetupDialog, SIGNAL(n75reqSetpointMap(quint8,quint8,quint8)), mds, SLOT(mdCmdReqN75SetpointMap(quint8,quint8,quint8)));
    connect (mds, SIGNAL( n75DutyMapreceived (quint8, quint8, quint8, QVector<quint8>*)), v2N75SetupDialog, SLOT(n75dutyMap(quint8,quint8,quint8,QVector<quint8>*)));
    connect (mds, SIGNAL( n75SetpointMapreceived (quint8, quint8, quint8, QVector<double>*)), v2N75SetupDialog, SLOT(n75SetpointMap(quint8,quint8,quint8,QVector<double>*)));
    connect (v2N75SetupDialog, SIGNAL(n75writeDutyMap(quint8,quint8,quint8,QVector<quint8>*)), mds, SLOT(mdCmdWriteN75DutyMap(quint8,quint8,quint8,QVector<quint8>*)));
    connect (v2N75SetupDialog, SIGNAL(n75writeSetpointMap(quint8,quint8,quint8,QVector<double>*)), mds, SLOT(mdCmdWriteN75SetpointMap(quint8,quint8,quint8,QVector<double>*)));

    connect (v2N75SetupDialog->ui->loadEepromPushButton, SIGNAL(clicked()), mds, SLOT(mdCmdLoadN75MapsFromEEprom()));
    connect (v2N75SetupDialog->ui->writeEepromPushButton, SIGNAL(clicked()), mds, SLOT(mdCmdWriteN75MapsToEEprom()));

    connect (v2N75SetupDialog->n75Settings, SIGNAL(requestN75PidSettings()), mds, SLOT(mdCmdReqN75Settings()));
    connect (mds, SIGNAL(n75SettingsReceived(quint8, double, double, double, double, double, double, double, double, bool, double)),
             v2N75SetupDialog->n75Settings, SLOT(n75PidSettings(quint8, double,double,double,double,double,double,double,double,bool,double)));
    connect (v2N75SetupDialog->n75Settings, SIGNAL(writeN75PidSettingsToEeprom()), mds, SLOT(mdCmdWriteN75SettingsToEEprom()));
    connect (v2N75SetupDialog->n75Settings, SIGNAL(readN75PidSettingsFromEeprom()), mds, SLOT(mdCmdReadN75SettingsFromEEprom()));
    connect (v2N75SetupDialog->n75Settings, SIGNAL(setN75PidSettings(quint8,double,double,double,double,double,double,double,double,bool,double)),
             mds, SLOT(mdCmdWriteN75Settings(quint8,double,double,double,double,double,double,double,double,bool,double)));
    connect (mds, SIGNAL(ackReceived (quint8)), v2N75SetupDialog, SLOT(ackReceived(quint8)));


    //N75 Pid settings on boost pid tab
    n75PidSettingsOnBoostTab = new N75PidSettingsWidget (pcmw);
    pcmw->ui.BoostParamFrame->setLayout( new QVBoxLayout(pcmw) );
    pcmw->ui.BoostParamFrame->layout()->addWidget(n75PidSettingsOnBoostTab);
    connect (n75PidSettingsOnBoostTab, SIGNAL(requestN75PidSettings()), mds, SLOT(mdCmdReqN75Settings()));
    connect (mds, SIGNAL(n75SettingsReceived(quint8, double, double, double, double, double, double, double, double, bool,double)),
             n75PidSettingsOnBoostTab, SLOT(n75PidSettings(quint8, double,double,double,double,double,double,double,double,bool,double)));
    connect (n75PidSettingsOnBoostTab, SIGNAL(writeN75PidSettingsToEeprom()), mds, SLOT(mdCmdWriteN75SettingsToEEprom()));
    connect (n75PidSettingsOnBoostTab, SIGNAL(readN75PidSettingsFromEeprom()), mds, SLOT(mdCmdReadN75SettingsFromEEprom()));
    connect (n75PidSettingsOnBoostTab, SIGNAL(setN75PidSettings(quint8,double,double,double,double,double,double,double,double,bool,double)),
             mds, SLOT(mdCmdWriteN75Settings(quint8,double,double,double,double,double,double,double,double,bool,double)));

    // Tests
//    demo1 = new CarPcDemoWidget (pcmw->ui.CarPcTab);

    //Dashboard
    rtvis = new RealTimeVis ( dynamic_cast<QWidget*>(pcmw->ui.DashboardTab) );
    connect (data, SIGNAL(rtNewDataRecord(MdDataRecord*)), rtvis, SLOT(visualize(MdDataRecord*)));
    connect (data, SIGNAL(showStatusMessage(QString)), rtvis, SLOT(showStatusMessage(QString)) );

    //Digifant-1 stuff
    connect (this, SIGNAL(newDfBoostTransferFunction(int)), pcmw, SLOT(newDfBoostTransferFunction(int)));
    connect (pcmw->ui.actionShow_application_window, SIGNAL(triggered()), dfAppWin, SLOT(show()));
    connect (data, SIGNAL(rtNewDataRecord(MdDataRecord*)), dfAppWin, SLOT(visualize(MdDataRecord*)));

    //navigate in vis / data record sheet
    connect (data, SIGNAL(showRecordInVis1 (int)), this, SLOT(changeDataWinMarkToDisplayRecord(int)));

    connect ( v2SettingsDialog, SIGNAL(cfgDialogAccepted()), rtvis, SLOT(possibleCfgChange()) );

//    Map16x1_NTC_IAT_ECT *map = new Map16x1_NTC_IAT_ECT();
//    map->testIt();
//    delete (map);
//    Map16x1_NTC_IAT *map = new Map16x1_NTC_IAT();
//    map->testIt();
//    delete (map);

}

void AppEngine::setupMobile() {
#ifdef Q_WS_MAEMO_5
    setupMaemo();
#endif
#ifdef Q_OS_ANDROID
    setupAndroid();
#endif
#if defined(Q_OS_IOS)
    setupIos();
#endif
}

void AppEngine::setupMaemo() {
    connect (mmw, SIGNAL(writeSettings()), this, SLOT(writeSettings()));

    connect (mmw->ui->actionVis1 , SIGNAL(triggered()), mvis1w, SLOT(show() ));
    connect (mmw->ui->actionBoostPid , SIGNAL(triggered()), mbw, SLOT(show() ));
    connect (mmw->ui->actionSerial, SIGNAL(triggered()), mySerialOptionsDialog, SLOT(show()));
    connect (mvis1w->ui->actionConfig_Visualization1, SIGNAL(triggered()), data, SLOT(showCfgVis1()) );
    connect (mmw->ui->actionSerialConnect, SIGNAL(triggered()), mds, SLOT(openPort() ) );
    connect (mmw->ui->actionSerialDisconnect, SIGNAL(triggered()), mds, SLOT(closePort() ) );

    connect (mmw->ui->actionSave_as, SIGNAL(triggered()), this, SLOT(saveDataAs() ) );
    connect (mmw->ui->actionOpen, SIGNAL(triggered()), this, SLOT(openData() ) );
    connect (mmw->ui->actionNew, SIGNAL(triggered()), this, SLOT(clearData() ) );
    connect (mmw->ui->actionSend_Commands, SIGNAL(triggered()), mcw, SLOT(show() ) );

    connect (data, SIGNAL(showStatusMessage(QString)), mmw, SLOT(showStatusMessage(QString)));
    //    connect (mds, SIGNAL(showStatusMessage(QString)), mmw, SLOT(showStatusMessage(QString)));
    connect (this, SIGNAL(showStatusMessage(QString)), mmw, SLOT(showStatusMessage(QString)));
//    connect (mds, SIGNAL(newBoostPidMsg(MdBoostPidRecord*)), this, SLOT(showBoostPidMsg(MdBoostPidRecord*)));

    //Evaluations
#ifndef ANDROID
    connect (mmw->ui->actionEvaluate, SIGNAL(triggered()), mevalDialog, SLOT(show()));
#endif

    connect (DataViewSlider, SIGNAL(valueChanged(int)), this, SLOT(changeDataWinMark(int)));
    connect (DataViewSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeDataWinSize(int)));
    connect (mvis1w->ui->DVMicroLeftButton, SIGNAL(clicked()), this, SLOT(changeDataWinMarkMicroLeft() ) );
    connect (mvis1w->ui->DVMicroRightButton, SIGNAL(clicked()), this, SLOT(changeDataWinMarkMicroRight() ) );
    connect (mvis1w->ui->DVLeftButton, SIGNAL(clicked()), this, SLOT(changeDVSliderDown()) );
    connect (mvis1w->ui->DVRightButton, SIGNAL(clicked()), this, SLOT(changeDVSliderUp()) );
    connect (mvis1w->ui->DelMarkerButton, SIGNAL(clicked()), data->getVisPlot(), SLOT (removeLastMarker()) );
    connect (mvis1w->ui->actionShow_last_records, SIGNAL(triggered()), this, SLOT (changeDVSliderMin()) );
    connect (mvis1w->ui->actionShow_first_records, SIGNAL(triggered()), this, SLOT (changeDVSliderMax()) );

    //replay
    connect (mvis1w->ui->ReplayPlayButton, SIGNAL(clicked()), this, SLOT(replayData()) );
//    TODO
    connect (mds, SIGNAL(portOpened()), mvis1w, SLOT(disableReplay()));
    connect (mds, SIGNAL(portClosed()), mvis1w, SLOT(enableReplay()));
    connect (mvis1w->ui->ReplayPauseButton, SIGNAL(clicked()), replay, SLOT(pause()) );
    connect (mvis1w->ui->ReplayStopButton, SIGNAL(clicked()), replay, SLOT(stop()) );
    connect (mvis1w->ui->ReplaySpinBox, SIGNAL(valueChanged(double)), this, SLOT(setReplaySpeedUpFactor(double)));


    //V2 settings
//    connect (mmw->ui->actionV2_N75_Settings, SIGNAL(triggered()), v2N75SetupDialog, SLOT(show()));
    connect (mmw->ui->actionV2_N75_Settings, SIGNAL(triggered()), v2N75SetupDialog, SLOT(show()));

    connect (mmw->ui->actionSettings, SIGNAL(triggered()), v2SettingsDialog, SLOT(show()));

    connect (v2N75SetupDialog, SIGNAL(n75reqDutyMap(quint8,quint8,quint8)), mds, SLOT(mdCmdReqN75DutyMap(quint8,quint8,quint8)));
    connect (v2N75SetupDialog, SIGNAL(n75reqSetpointMap(quint8,quint8,quint8)), mds, SLOT(mdCmdReqN75SetpointMap(quint8,quint8,quint8)));
    connect (mds, SIGNAL( n75DutyMapreceived (quint8, quint8, quint8, QVector<quint8>*)), v2N75SetupDialog, SLOT(n75dutyMap(quint8,quint8,quint8,QVector<quint8>*)));
    connect (mds, SIGNAL( n75SetpointMapreceived (quint8, quint8, quint8, QVector<double>*)), v2N75SetupDialog, SLOT(n75SetpointMap(quint8,quint8,quint8,QVector<double>*)));
    connect (v2N75SetupDialog, SIGNAL(n75writeDutyMap(quint8,quint8,quint8,QVector<quint8>*)), mds, SLOT(mdCmdWriteN75DutyMap(quint8,quint8,quint8,QVector<quint8>*)));
    connect (v2N75SetupDialog, SIGNAL(n75writeSetpointMap(quint8,quint8,quint8,QVector<double>*)), mds, SLOT(mdCmdWriteN75SetpointMap(quint8,quint8,quint8,QVector<double>*)));

    connect (v2N75SetupDialog->ui->loadEepromPushButton, SIGNAL(clicked()), mds, SLOT(mdCmdLoadN75MapsFromEEprom()));
    connect (v2N75SetupDialog->ui->writeEepromPushButton, SIGNAL(clicked()), mds, SLOT(mdCmdWriteN75MapsToEEprom()));

    connect (v2N75SetupDialog->n75Settings, SIGNAL(requestN75PidSettings()), mds, SLOT(mdCmdReqN75Settings()));
    connect (mds, SIGNAL(n75SettingsReceived(quint8, double, double, double, double, double, double, double, double, bool,double)),
             v2N75SetupDialog->n75Settings, SLOT(n75PidSettings(quint8, double,double,double,double,double,double,double,double,bool,double)));
    connect (v2N75SetupDialog->n75Settings, SIGNAL(writeN75PidSettingsToEeprom()), mds, SLOT(mdCmdWriteN75SettingsToEEprom()));
    connect (v2N75SetupDialog->n75Settings, SIGNAL(readN75PidSettingsFromEeprom()), mds, SLOT(mdCmdReadN75SettingsFromEEprom()));

    connect (v2N75SetupDialog->n75Settings, SIGNAL(setN75PidSettings(quint8,double,double,double,double,double,double,double,double,bool,double)),
             mds, SLOT(mdCmdWriteN75Settings(quint8,double,double,double,double,double,double,double,double,bool,double)));

    connect (mds, SIGNAL(ackReceived (quint8)), v2N75SetupDialog, SLOT(ackReceived(quint8)));

    connect (mmw->ui->actionGearbox_settings, SIGNAL(triggered()), gearSettingsDialog, SLOT(show()));
    connect (mmw->ui->actionAbout, SIGNAL(triggered()), aboutDialog, SLOT(show()));

    //Dashboard
    mmw->ui->mainFrame->setContentsMargins(0,0,0,0);
//    QPalette pal = mmw->ui->mainFrame->palette();
//    pal.setColor(mmw->ui->mainFrame->backgroundRole(), Qt::red);
//    mmw->ui->mainFrame->setPalette(pal);

    rtvis = new RealTimeVis ( mmw->ui->mainFrame );
    connect (data, SIGNAL(rtNewDataRecord(MdDataRecord*)), rtvis, SLOT(visualize(MdDataRecord*)));

    connect ( v2SettingsDialog, SIGNAL(cfgDialogAccepted()), rtvis, SLOT(possibleCfgChange()) );

#if  defined (Q_WS_MAEMO_5)
    QSettings settings("MultiDisplay", "UI");
    if ( settings.value("mobile/use_gps", QVariant(true)).toBool() )
        mGps = new MobileGPS (this);
    else
        mGps = nullptr;
    if ( settings.value("mobile/use_accel", QVariant(true)).toBool() )
        accelMeter = new Accelerometer(this);
    else
        accelMeter = nullptr;
#else
    mGps = nullptr;
    accelMeter = nullptr;
#endif

    //load TEST-DATA
//   openData ("/home/developer/md/run1");
}


void AppEngine::setupAndroid () {
#if defined Q_OS_ANDROID
    QSettings s;
    qDebug() << "settings are stored in " << s.fileName();

    connect (amw, SIGNAL(writeSettings()), this, SLOT(writeSettings()));

    //Evaluations

    //Vis1 (Graph)

    //replay

    //Bluetooth
    connect (mds, SIGNAL(portOpened()), mvis1w, SLOT(disableReplay()));
    connect (mds, SIGNAL(portOpened()), this, SLOT(androidStartLocationQuery()));
    connect (mds, SIGNAL(portClosed()), mvis1w, SLOT(enableReplay()));

    //V2 settings
    AndroidN75Dialog* an75 = new AndroidN75Dialog (amw, mds);
    connect (amw->ui->actionV2_N75_Settings, SIGNAL(triggered()), an75, SLOT(showMaximized()));

    //TODO FIXME: refactor menu!
    /*
    delete (amw->ui->menuConfig);
    amw->ui->menuConfig = nullptr;
    delete ( amw->ui->actionSettings );
    amw->ui->actionSettings = nullptr;
    */
    connect (amw->ui->actionSettings, SIGNAL(triggered()), v2SettingsDialog, SLOT(showMaximized()));

    //TODO dont works! :(
    //QAction *a = new QAction("aha", amw->ui->menubar);
    //amw->ui->menubar->addAction(a);
    //connect(a, SIGNAL(triggered()), v2SettingsDialog, SLOT(showMaximized()));

    //    connect (amw->ui->actionV2_N75_Settings, SIGNAL(triggered()), v2N75SetupDialog, SLOT(showMaximized()));
//    connect (v2N75SetupDialog, SIGNAL(n75reqDutyMap(quint8,quint8,quint8)), mds, SLOT(mdCmdReqN75DutyMap(quint8,quint8,quint8)));
//    connect (v2N75SetupDialog, SIGNAL(n75reqSetpointMap(quint8,quint8,quint8)), mds, SLOT(mdCmdReqN75SetpointMap(quint8,quint8,quint8)));
//    connect (mds, SIGNAL( n75DutyMapreceived (quint8, quint8, quint8, QVector<quint8>*)), v2N75SetupDialog, SLOT(n75dutyMap(quint8,quint8,quint8,QVector<quint8>*)));
//    connect (mds, SIGNAL( n75SetpointMapreceived (quint8, quint8, quint8, QVector<double>*)), v2N75SetupDialog, SLOT(n75SetpointMap(quint8,quint8,quint8,QVector<double>*)));
//    connect (v2N75SetupDialog, SIGNAL(n75writeDutyMap(quint8,quint8,quint8,QVector<quint8>*)), mds, SLOT(mdCmdWriteN75DutyMap(quint8,quint8,quint8,QVector<quint8>*)));
//    connect (v2N75SetupDialog, SIGNAL(n75writeSetpointMap(quint8,quint8,quint8,QVector<double>*)), mds, SLOT(mdCmdWriteN75SetpointMap(quint8,quint8,quint8,QVector<double>*)));

//    connect (v2N75SetupDialog->ui->loadEepromPushButton, SIGNAL(clicked()), mds, SLOT(mdCmdLoadN75MapsFromEEprom()));
//    connect (v2N75SetupDialog->ui->writeEepromPushButton, SIGNAL(clicked()), mds, SLOT(mdCmdWriteN75MapsToEEprom()));

//    connect (v2N75SetupDialog->n75Settings, SIGNAL(requestN75PidSettings()), mds, SLOT(mdCmdReqN75Settings()));
//    connect (mds, SIGNAL(n75SettingsReceived(quint8, double, double, double, double, double, double, double, double, bool,double)),
//             v2N75SetupDialog->n75Settings, SLOT(n75PidSettings(quint8, double,double,double,double,double,double,double,double,bool,double)));
//    connect (v2N75SetupDialog->n75Settings, SIGNAL(writeN75PidSettingsToEeprom()), mds, SLOT(mdCmdWriteN75SettingsToEEprom()));
//    connect (v2N75SetupDialog->n75Settings, SIGNAL(readN75PidSettingsFromEeprom()), mds, SLOT(mdCmdReadN75SettingsFromEEprom()));
//    connect (v2N75SetupDialog->n75Settings, SIGNAL(setN75PidSettings(quint8,double,double,double,double,double,double,double,double,bool,double)),
//             mds, SLOT(mdCmdWriteN75Settings(quint8,double,double,double,double,double,double,double,double,bool,double)));
//    connect (mds, SIGNAL(ackReceived (quint8)), v2N75SetupDialog, SLOT(ackReceived(quint8)));

    connect (amw->ui->actionGearbox_settings, SIGNAL(triggered()), gearSettingsDialog, SLOT(showMaximized()) );
    connect (amw->ui->actionAbout, SIGNAL(triggered()), aboutDialog, SLOT(showMaximized()));



    //Dashboard
    //test: moved to AndroidDashboardDialog
    //amw->ui->mainFrame->setContentsMargins(0,0,0,0);

    add = new AndroidDashboardDialog( );
    //rtvis = new RealTimeVis ( add );
    rtvis = new RealTimeVis ( amw );
    connect (data, SIGNAL(rtNewDataRecord(MdDataRecord*)), rtvis, SLOT(visualize(MdDataRecord*)));

    //test: moved to AndroidDashboardDialog
    /*
    if ( QAndroidJniObject::callStaticMethod<jboolean>( "de/gummelinformatics/mui/MuiIntentHelper", "hasPermanentMenuKey" ))
        connect (amw->ui->dashboardPushButton, SIGNAL(clicked()), add, SLOT(showFullScreen()) );
    else
        connect (amw->ui->dashboardPushButton, SIGNAL(clicked()), add, SLOT(showMaximized()) );
    */

    connect (rtvis, SIGNAL(showStatusMessage(QString)), amw, SLOT(showStatusMessage(QString)) );
    connect (mdcom, SIGNAL(showStatusMessage(QString)), amw, SLOT(showStatusMessage(QString)) );
    connect (mds, SIGNAL(showStatusMessage(QString)), amw, SLOT(showStatusMessage(QString)) );
    connect (this, SIGNAL(showStatusMessage(QString)), amw, SLOT(showStatusMessage(QString)) );
    connect (data, SIGNAL(showStatusMessage(QString)), amw, SLOT(showStatusMessage(QString)) );
    connect (mdcom, SIGNAL(portClosed()), amw, SLOT(btPortClosed()) );
    connect (mdcom, SIGNAL(portOpened()), amw, SLOT(btPortOpened()) );

    connect (amw->ui->actionBluetoothToggleState, SIGNAL(triggered()), mdcom, SLOT(togglePort()) );
    connect (amw->ui->actionSave, SIGNAL(triggered()) , this, SLOT(saveDataAs()) );
    connect (amw->ui->actionOpen_Replay, SIGNAL(triggered()) , this, SLOT(openData()) );
    connect (amw->ui->actionClear, SIGNAL(triggered()) , this, SLOT(clearData()) );


    QSettings settings;
    //if ( settings.value("mobile/use_gps", QVariant(true)).toBool() )
    //    mGps = new MobileGPS (this);
    //else
    mGps = nullptr;
    if ( settings.value("mobile/use_accel", QVariant(true)).toBool() )
        accelMeter = new Accelerometer(this);
    else
        accelMeter = nullptr;

    connect ( v2SettingsDialog, SIGNAL(cfgDialogAccepted()), rtvis, SLOT(possibleCfgChange()) );

    amw->showStatusMessage("Bluetooth autoconnect is ON");

    //http://qt-project.org/doc/qt-5/qandroidjniobject.html#details
//    QAndroidJniObject activity = QtAndroid::androidActivity();
//    Q_ASSERT ( activity.isValid() );
//    activity.callMethod<void>("setKeepScreenOn", "(B)V", true);
#endif
}

void AppEngine::setupIos() {
#if defined (Q_OS_IOS)
    QSettings s;
    qDebug() << "settings are stored in " << s.fileName();

    connect (amw, SIGNAL(writeSettings()), this, SLOT(writeSettings()));

    //Evaluations

    //Vis1 (Graph)

    //replay

    //Bluetooth
    connect (mds, SIGNAL(portOpened()), mvis1w, SLOT(disableReplay()));
    connect (mds, SIGNAL(portOpened()), this, SLOT(androidStartLocationQuery()));
    connect (mds, SIGNAL(portClosed()), mvis1w, SLOT(enableReplay()));

    //V2 settings
    /*
    AndroidN75Dialog* an75 = new AndroidN75Dialog (amw, mds);
    connect (amw->ui->actionV2_N75_Settings, SIGNAL(triggered()), an75, SLOT(showMaximized()));
*/
    //TODO FIXME: refactor menu!
    /*
    delete (amw->ui->menuConfig);
    amw->ui->menuConfig = nullptr;
    delete ( amw->ui->actionSettings );
    amw->ui->actionSettings = nullptr;
    */
    connect (amw->ui->actionSettings, SIGNAL(triggered()), v2SettingsDialog, SLOT(showMaximized()));

    connect (amw->ui->actionGearbox_settings, SIGNAL(triggered()), gearSettingsDialog, SLOT(showMaximized()) );
    connect (amw->ui->actionAbout, SIGNAL(triggered()), aboutDialog, SLOT(showMaximized()));

    add = new AndroidDashboardDialog( );
    //rtvis = new RealTimeVis ( add );
    rtvis = new RealTimeVis ( amw );
    connect (data, SIGNAL(rtNewDataRecord(MdDataRecord*)), rtvis, SLOT(visualize(MdDataRecord*)));

    connect (mdcom, SIGNAL(showStatusMessage(QString)), amw, SLOT(showStatusMessage(QString)) );
    connect (mds, SIGNAL(showStatusMessage(QString)), amw, SLOT(showStatusMessage(QString)) );
    connect (this, SIGNAL(showStatusMessage(QString)), amw, SLOT(showStatusMessage(QString)) );
    connect (mdcom, SIGNAL(showStatusMessage(QString)), rtvis, SLOT(showStatusMessage(QString)) );
    connect (mds, SIGNAL(showStatusMessage(QString)), rtvis, SLOT(showStatusMessage(QString)) );
    connect (this, SIGNAL(showStatusMessage(QString)), rtvis, SLOT(showStatusMessage(QString)) );
    connect (data, SIGNAL(showStatusMessage(QString)), rtvis, SLOT(showStatusMessage(QString)) );
    connect (mdcom, SIGNAL(portClosed()), amw, SLOT(btPortClosed()) );
    connect (mdcom, SIGNAL(portOpened()), amw, SLOT(btPortOpened()) );

    connect (amw->ui->actionBluetoothToggleState, SIGNAL(triggered()), mdcom, SLOT(togglePort()) );
    connect (amw->ui->actionSave, SIGNAL(triggered()) , this, SLOT(saveDataAs()) );
    connect (amw->ui->actionOpen_Replay, SIGNAL(triggered()) , this, SLOT(openData()) );
    connect (amw->ui->actionClear, SIGNAL(triggered()) , this, SLOT(clearData()) );


    QSettings settings;
    if ( settings.value("mobile/use_gps", QVariant(true)).toBool() )
        mGps = new MobileGPS (this);
    else
        mGps = nullptr;
    if ( settings.value("mobile/use_accel", QVariant(true)).toBool() )
        accelMeter = new Accelerometer(this);
    else
        accelMeter = nullptr;

    connect ( v2SettingsDialog, SIGNAL(cfgDialogAccepted()), rtvis, SLOT(possibleCfgChange()) );

    rtvis->showStatusMessage("Bluetooth autoconnect");
#endif
}

void AppEngine::reCreateDialogsAndroidFix()
{
#ifdef Q_OS_ANDROID
    if ( v2SettingsDialog == nullptr ) {
        v2SettingsDialog = new V2SettingsDialog(amw);
        connect (amw->ui->actionSettings, SIGNAL(triggered()), v2SettingsDialog, SLOT(showMaximized()));
        connect ( v2SettingsDialog, SIGNAL(cfgDialogAccepted()), rtvis, SLOT(possibleCfgChange()) );
    }
    if ( aboutDialog == nullptr ) {
        aboutDialog = new AboutDialog(amw);
        connect (amw->ui->actionAbout, SIGNAL(triggered()), aboutDialog, SLOT(showMaximized()));
    }
    if ( gearSettingsDialog == nullptr ) {
        gearSettingsDialog = new GearSettingsDialog(amw);
        connect (amw->ui->actionGearbox_settings, SIGNAL(triggered()), gearSettingsDialog, SLOT(showMaximized()));
    } else {
        connect (amw->ui->actionGearbox_settings, SIGNAL(triggered()), gearSettingsDialog, SLOT(showMaximized()));
    }
    if ( v2N75SetupDialog == nullptr ) {
        v2N75SetupDialog = new V2N75SetupDialog(amw);
        connect (amw->ui->actionV2_N75_Settings, SIGNAL(triggered()), v2N75SetupDialog, SLOT(showMaximized()));
    }
#endif
}


void AppEngine::show() {
#if  !defined (Q_WS_MAEMO_5)  && !defined (Q_OS_ANDROID) && !defined (Q_OS_IOS)
    //Windows / Linux Desktop GUI
    pcmw->show();
#endif
#if  defined (Q_WS_MAEMO_5)
    //Maemo Stacked Windows
    mmw->showMaximized();
    mmw->statusBar()->hide();
#endif

#if defined (Q_OS_ANDROID)
    jboolean permanentMenuKey = QAndroidJniObject::callStaticMethod<jboolean>( "de/gummelinformatics/mui/MuiIntentHelper", "hasPermanentMenuKey" );
    qDebug() << "hasPermanentMenuKey result: " << ( permanentMenuKey==true ? "true" : "false" );
    if ( permanentMenuKey ) {
        amw->showFullScreen();
        //add->showFullScreen();
    } else {
        amw->showMaximized();
        //add->showMaximized();
    }
 #endif

#if defined (Q_OS_IOS)
    amw->showMaximized();
    //amw->showFullScreen();
#endif
}

void AppEngine::saveData () {
    QString path;
#if QT_VERSION >= 0x050000
#if not defined (ANDROID)
    path =  QStandardPaths::standardLocations (QStandardPaths::DocumentsLocation)[0]
            + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".mdv2";
#endif
#if defined (ANDROID)
    //QAndroidJniObject s = QAndroidJniObject::callStaticObjectMethod( "de/gummelinformatics/mui/MuiIntentHelper", "getPublicDocumentPath", "()Ljava/lang/String;" );
    QAndroidJniObject s = QAndroidJniObject::callStaticObjectMethod( "de/gummelinformatics/mui/MuiIntentHelper", "getPublicDownloadsPath", "()Ljava/lang/String;" );
    path = s.toString() + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".mdv2";
    qDebug() << "android save path 2020: " << path;
#endif

#else
    //Qt 4.x
    path = QDesktopServices::storageLocation (QDesktopServices::DocumentsLocation)
            + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".mdv2";
#endif

    data->saveData(path);
/*
* disabled 2020-06: we already store gps and accel reading in MdDataRecord!
#if  defined (Q_WS_MAEMO_5)  || defined (Q_OS_ANDROID)
    if ( mGps ) {
        mGps->saveTrack (path + ".track");
        mGps->saveTrackBinary (path + ".trackB");
    }
#endif
*/
}

void AppEngine::saveDataAsCSV() {
    QString path;
#if QT_VERSION >= 0x050000
#if not defined (ANDROID)
    path =  QStandardPaths::standardLocations (QStandardPaths::DocumentsLocation)[0]
            + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".csv";
#endif
#if defined (ANDROID)
    QAndroidJniObject s = QAndroidJniObject::callStaticObjectMethod( "de/gummelinformatics/mui/MuiIntentHelper", "getPublicDocumentPath", "()Ljava/lang/String;" );
    path = s.toString() + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".mdv2";
    qDebug() << "android save path 2019: " << path;
#endif
#else
    //Qt 4.x
    path = QDesktopServices::storageLocation (QDesktopServices::DocumentsLocation)
            + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".csv";
#endif

    /*
#if defined (ANDROID)
    //Hack for sdcard
    path = QString("/scard") + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".mdv2";
#endif
*/
#if defined (Q_OS_LINUX)
    QString fn = QFileDialog::getSaveFileName ( pcmw, QString("Select Filename"), path,
                                                "CSV (*.csv)", nullptr, QFileDialog::DontUseNativeDialog);
#else
    QString fn = QFileDialog::getSaveFileName ( pcmw, QString("Select Filename"), path,
                                                "CSV (*.csv)");
#endif
    if ( fn != "")
        data->saveDataCSV(fn);
}

void AppEngine::saveDataAs () {
    QString path;
#if QT_VERSION >= 0x050000
#if not defined (ANDROID)
    path =  QStandardPaths::standardLocations (QStandardPaths::DocumentsLocation)[0]
            + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".mdv2";
#endif
#if defined (ANDROID)
    //QAndroidJniObject s = QAndroidJniObject::callStaticObjectMethod( "de/gummelinformatics/mui/MuiIntentHelper", "getPublicDocumentPath", "()Ljava/lang/String;" );
    QAndroidJniObject s = QAndroidJniObject::callStaticObjectMethod( "de/gummelinformatics/mui/MuiIntentHelper", "getPublicDownloadsPath", "()Ljava/lang/String;" );
    path = s.toString() + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".mdv2";
    qDebug() << "android save path 2019: " << path;
#endif

#else
    //Qt 4.x
    path = QDesktopServices::storageLocation (QDesktopServices::DocumentsLocation)
            + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".mdv2";
#endif

#if defined (ANDROID)
    QString fn = path;
    //QString fn = QFileDialog::getSaveFileName( amw, QString("Select File"), path, "mdv2 (*.mdv2)" );
    //dont show select window because there is no native android dialog -> qt desktop dialog sucks on mobile
    qDebug() << "android: save file instantly to " + path;
    //TODO FIXME Android: use native save dialog !
    //https://codereview.qt-project.org/c/qt/qtbase/+/251238
    emit showStatusMessage( "save to " + fn );
#else
#if defined (Q_OS_LINUX)
    QString fn = QFileDialog::getSaveFileName ( pcmw, QString("Select Filename"), path,
                                                "mdv2 (*.mdv2)", nullptr, QFileDialog::DontUseNativeDialog);
#else
    QString fn = QFileDialog::getSaveFileName ( pcmw, QString("Select Filename"), path,
                                                "mdv2 (*.mdv2)");
#endif
#endif
    if ( fn != "") {
        if ( ! data->saveData(fn) ) {
            qDebug() << "save failed!";
            emit showStatusMessage( "save failed!" );
        } else {
#if defined (ANDROID)
            QAndroidJniObject jsPath = QAndroidJniObject::fromString(path);
            QAndroidJniObject jsTitle = QAndroidJniObject::fromString("digifant log");
            QAndroidJniObject jsMimeType = QAndroidJniObject::fromString("application/digifant-ecu");
            int requestId = 0;
            /*
                        QAndroidJniObject::callStaticObjectMethod("de/gummelinformatics/mui/MuiIntentHelper",
                                                                  "sendFile",
                                                                  "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I;Landroid/content/Context;)V",
                                                                  jsPath.object<jstring>(), jsTitle.object<jstring>(), jsMimeType.object<jstring>(), requestId, QtAndroid::androidContext().object() );
                                                                  */
            // JNI DETECTED ERROR IN APPLICATION: use of invalid jobject 0xc131ba50
            //QAndroidJniObject::callStaticMethod<void>("de/gummelinformatics/mui/MuiIntentHelper",
            //                                          "sendFileSimple",
            //                                          "(Ljava/lang/String;Landroid/content/Context;)V", path.toLocal8Bit().data(), QtAndroid::androidContext().object() );

#endif
        }
/*
 * disabled 2020-06: we already store gps and accel reading in MdDataRecord!
#if  defined (Q_WS_MAEMO_5)  || defined (ANDROID)
        if ( mGps ) {
            mGps->saveTrack (fn + ".track");
            mGps->saveTrackBinary (fn + ".trackB");
        }
#endif
*/
    }
}
void AppEngine::openData ( QString fn ) {
    if ( fn == "" ) {
        //, QString("~"), QString("*.mdd")
//        QString path = QDesktopServices::storageLocation (QDesktopServices::DocumentsLocation);
#if defined (Q_OS_LINUX)
        fn = QFileDialog::getOpenFileName ( pcmw, tr("Select Filename"), directory, tr("mdv2 (*.mdv2)"), nullptr, QFileDialog::DontUseNativeDialog);
#else
        fn = QFileDialog::getOpenFileName ( pcmw, tr("Select Filename"), directory, tr("mdv2 (*.mdv2)"), nullptr);
#endif
    }

//#if defined (ANDROID)
//    //Hack for sdcard
//    fn = QString("/scard");
//#endif


    if ( ! fn.isNull() ) {
        directory = QFileInfo(fn).path(); // store path for next time
        qDebug() << "set new directory " << directory;

#ifdef Q_WS_MAEMO5
        mmw->setAttribute(Qt::WA_Maemo5ShowProgressIndicator);
#endif
        mds->closePort();
        //clearData();

        dashboardActualizeSave = getActualizeDashboard() ;
        vis1ActualizeSave = getActualizeVis1();
        setActualizeDashboard( false );
        setActualizeVis1( false );

        data->loadData(fn);

        setActualizeDashboard( dashboardActualizeSave );
        setActualizeVis1( vis1ActualizeSave );

#ifdef Q_WS_MAEMO_5
        mmw->setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif

#if not defined Q_WS_MAEMO_5 and not defined Q_OS_ANDROID and not defined Q_OS_IOS
        emit showStatusBarSampleCount( QString::number(data->size()) );
#endif

#if defined Q_OS_ANDROID or defined Q_OS_IOS
        //start replay on android
        replayData();
#else
        DataViewSlider->setValue(0);
        DataViewSpinBox->setValue(100);
        changeDataWinSize( 100 );
#endif
    }
}

void AppEngine::clearData () {
    mds->closePort();
    data->clearData();

#if defined Q_OS_ANDROID or defined Q_OS_IOS
    if ( replay && ( replayThread->isRunning() || replayThread->isFinished() ) )
        replay->stop();
#endif
#if not defined Q_WS_MAEMO_5 and not defined Q_OS_ANDROID and not defined Q_OS_IOS
    emit showStatusMessage( QString::number(data->size()) );
#endif
}

void AppEngine::changeSerialOptions() {
    bool isBt = false;
    if ( mySerialOptionsDialog->getUi()->portComboBox->currentText() == "bluetooth" ) {
        isBt = true;
        if ( qobject_cast<MdBluetoothWrapper*>(mds) == nullptr ) {
            mds->changeComInstance( new MdBluetoothWrapper(this) );
        }
    } else {
#if not defined (Q_OS_IOS)
        if ( qobject_cast<MdQSerialPortCom*>(mds) == nullptr ) {
            mds->changeComInstance( new MdQSerialPortCom(this) );
        }
#endif
    }
    //TODO mac osx dynamic names like /dev/tty.usbserial-A900JFA
    if ( !isBt )
        mds->changePortSettings ( mySerialOptionsDialog->getUi()->portComboBox->currentText(), mySerialOptionsDialog->getUi()->speedComboBox->currentText() );
}

void AppEngine::changeDataWinMarkMicroLeft () {
    data->changeDataWinMarkMicroRelative(10, true, DataViewSlider->maximum());
}

void AppEngine::changeDataWinMarkMicroRight () {
    data->changeDataWinMarkMicroRelative(10, false, DataViewSlider->maximum());
}

void AppEngine::changeDataWinMarkToDisplayRecord(const int record) {
    DataViewSlider->setValue( data->changeDataWinMarkToDisplayRecord(record, DataViewSlider->maximum() ) );
}

void AppEngine::changeDataWinMark(const int &nwm) {
    data->changeDataWinMark(nwm, DataViewSlider->maximum() );
}

void AppEngine::changeDataWinSize (int ns) {

    if ( ns > data->size() ) {
        ns = data->size();
        //        pcmw->ui.DataViewWinSizeSpinBox->setValue(ns);
        DataViewSpinBox->setValue(ns);
    }

    //compute new scale for the slider!
    int scaleoldmax =  DataViewSlider->maximum();
    int scaleoldval =  DataViewSlider->value();
    int currentmarkerpos = (data->size() / scaleoldmax) * scaleoldval;

    int scalenewmax = data->size() / ns ;
    //	scalenewmax++;
    scalenewmax += ceil (scalenewmax * 0.1);

    int newmarkerpos = (currentmarkerpos * scalenewmax) / data->size();

    DataViewSlider->setMaximum(scalenewmax);
    DataViewSlider->setValue(newmarkerpos);

    data->changeDataWinMark(newmarkerpos, scalenewmax);
    data->changeDataWinSize( ns );

    //	qDebug() << "changeDataWinSize new size=" << ns << " data->size()=" << data->size() << " marker max=" << scalenewmax << " old max=" << scaleoldmax << " currentmarkerps=" << currentmarkerpos << " newmarkerpos=" << newmarkerpos;

}


void AppEngine::writeSettings () {
    QSettings settings;

#if not defined Q_WS_MAEMO_5 and not defined Q_OS_ANDROID and not defined Q_OS_IOS
    settings.beginGroup("MainWindow");
    settings.setValue("size", pcmw->size());
    settings.setValue("pos", pcmw->pos());
    settings.setValue("datatabindex", pcmw->ui.DataTableWidget->currentIndex());
    settings.endGroup();
#endif

    if ( data->getVisPlot() )
        data->getVisPlot()->writeSettings();

    //FIXME -> V2
//    settings.beginGroup("boostpid");
//    settings.setValue ("setpoint", BoostPIDTableWidget->item(1,0)->text());
//    settings.setValue ("output", BoostPIDTableWidget->item(1,2)->text());
//    settings.setValue ("p", BoostPIDTableWidget->item(1,3)->text());
//    settings.setValue ("i", BoostPIDTableWidget->item(1,4)->text());
//    settings.setValue ("d", BoostPIDTableWidget->item(1,5)->text());
//    settings.setValue ("mode", BoostPIDTableWidget->item(1,6)->text());
//    settings.endGroup();

    settings.setValue ("mdserial/port", mySerialOptionsDialog->getUi()->portComboBox->currentIndex() );
    settings.setValue ("mdserial/speed", mySerialOptionsDialog->getUi()->speedComboBox->currentIndex() );

    settings.setValue ("md/actualizeVis1", getActualizeVis1() );
    settings.setValue ("md/actualizeDashboard", getActualizeDashboard() );

    settings.setValue ("md/num_connected_typek", numConnectedTypeK);

    settings.beginGroup ("Digifant1");
    settings.setValue ("MapSensor", dfBoostTransferFunction->name() );
    settings.endGroup();

#if  defined (Q_WS_MAEMO_5) || defined (Q_OS_ANDROID) || defined (Q_OS_IOS)
    if ( mGps )
        settings.setValue("mobile/use_gps", QVariant(true) );
    else
        settings.setValue("mobile/use_gps", QVariant(false) );

    if ( accelMeter )
        settings.setValue("mobile/use_accel", QVariant(true));
    else
        settings.setValue("mobile/use_accel", QVariant(false));
#endif


#if defined (Q_OS_ANDROID)
    //TODO fixme save bluetooth config
#endif

    data->writeSettings();
}



void AppEngine::readSettings () {
    QSettings settings("MultiDisplay", "UI");

#if !defined(Q_WS_MAEMO_5) && !defined(ANDROID) && !defined(Q_OS_IOS)
    settings.beginGroup("MainWindow");
    pcmw->resize(settings.value("size", QSize(800, 480)).toSize());
    pcmw->move(settings.value("pos", QPoint(200, 200)).toPoint());
    pcmw->ui.DataTableWidget->setCurrentIndex(settings.value ("datatabindex", 1).toInt());
    settings.endGroup();
#endif

    if ( data->getVisPlot() )
        data->getVisPlot()->readSettings();


    mySerialOptionsDialog->getUi()->portComboBox->setCurrentIndex( settings.value ("mdserial/port", 0).toInt() );
    mySerialOptionsDialog->getUi()->speedComboBox->setCurrentIndex( settings.value ("mdserial/speed", 0).toInt() );
#if not defined ( Q_OS_ANDROID ) && not defined (Q_OS_IOS)
    changeSerialOptions();
    //mds->changePortSettings( mySerialOptionsDialog->getUi()->portComboBox->currentText(), mySerialOptionsDialog->getUi()->speedComboBox->currentText() );
#else
    //HACK FIXME to open the spp profile with starting name mdv2
    mds->changePortSettings ("", 0);
#endif

    setActualizeVis1( settings.value ("md/actualizeVis1", true).toBool() );
    setActualizeDashboard( settings.value ("md/actualizeDashboard", true).toBool() );

    numConnectedTypeK = settings.value ("md/num_connected_typek", 5).toInt();

    settings.beginGroup ("Digifant1");
    int dfMap = settings.value ("MapSensor", 200).toInt();
    switch (dfMap) {
    case 400 :
        dfBoostTransferFunction = new TransferFunction400kpa();
        break;
    case 250 :
        dfBoostTransferFunction = new TransferFunction250kpa();
        break;
    case 100 :
        dfBoostTransferFunction = new TransferFunction100kpa();
        break;
    case 300 :
        dfBoostTransferFunction = new TransferFunction300kpa();
        break;
    case 200 :
    default:
        dfBoostTransferFunction = new TransferFunction200kpa();
        break;
    }
    emit newDfBoostTransferFunction (dfBoostTransferFunction->name());
    settings.endGroup();

    data->readSettings();

    qDebug() << "loaded settings (serial port " << mySerialOptionsDialog->getUi()->portComboBox->currentText() << ")";
}

void AppEngine::closeEvent(QCloseEvent *event) {
    writeSettings();
    event->accept();
    pcmw->QMainWindow::closeEvent ( event );
}


void AppEngine::replayData() {
#if defined Q_WS_MAEMO_5 and not defined Q_OS_ANDROID and not defined Q_OS_IOS
    replaySpeedUpFactor = mvis1w->ui->ReplaySpinBox->value();
    replayStartAtPos = data->getVisPlot()->windowBegin();
#endif
#if not defined Q_WS_MAEMO_5 and not defined Q_OS_ANDROID and not defined Q_OS_IOS
    if ( !pcmw->ui.ReplayCurPos->isChecked() )
        replayStartAtPos = 0;
    else
        replayStartAtPos = data->getVisPlot()->windowBegin();
#endif
//    replayThreadStopRequested = false;

    replayThread->start();

#if defined Q_WS_MAEMO_5 and not defined Q_OS_ANDROID and not defined Q_OS_IOS
    DataViewSlider->setValue( DataViewSlider->minimum() );
#endif
}

void AppEngine::changeDVSliderUp() {
#ifdef Q_WS_MAEMO_5
    DataViewSlider->setValue( DataViewSlider->value() - DataViewSlider->singleStep() );
#endif
}
void AppEngine::changeDVSliderDown() {
#ifdef Q_WS_MAEMO_5
    DataViewSlider->setValue( DataViewSlider->value() + DataViewSlider->singleStep() );
#endif
}

void AppEngine::changeDVSliderMin() {
    DataViewSlider->setValue( DataViewSlider->minimum() );
}
void AppEngine::changeDVSliderMax() {
    DataViewSlider->setValue( DataViewSlider->maximum() );
}

void AppEngine::androidStartLocationQuery() {
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    QSettings settings;
    if ( settings.value("mobile/use_gps", QVariant(true)).toBool() ) {
        mGps = new MobileGPS (this);
        qDebug() << "created MobileGPS instance";
    } else
      mGps = nullptr;
#endif
}
