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
#include <QDesktopWidget>
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

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) && not defined(Q_OS_ANDROID)
    #include "com/MdQSerialPortCom.h"
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    #include "com/MdQextSerialCom.h"
#endif

#if defined (Q_OS_ANDROID)
    #include "com/MdBluetoothCom.h"
    #include <QtAndroid>
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

#if  !defined (Q_WS_MAEMO_5)  && !defined (ANDROID)
    qDebug() << "desktop version";
    pcmw = new MultidisplayUIMainWindow ();
    mmw = NULL;
    mbw = NULL;
    mvis1w = NULL;
    mcw = NULL;
    amw = NULL;

    DataViewSlider = pcmw->ui.DataViewSlider;
    DataViewSpinBox = pcmw->ui.DataViewWinSizeSpinBox;

    data = new MdData(pcmw, pcmw->ui.BoostGraphGroupBox, pcmw, pcmw->ui.VisualizationTab, pcmw->ui.DataTableView );

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    mdcom = new MdQSerialPortCom(this);
#else
    mdcom = new MdQextSerialCom(this);
#endif
    mds = new MdBinaryProtocol(this, data, mdcom);

    mySerialOptionsDialog = new SerialOptionsDialog ();

    evalWinBoostLambda = new EvaluationWindow (NULL, data, EvaluationWindow::BoostLambda );
    evalWinRPMBoost = new EvaluationWindow (NULL, data, EvaluationWindow::RPMBoost );
    //	evalWinBoostLambdaSpectro = new EvaluationWindow (NULL, data, EvaluationWindow::SpectroBoostLambda );
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
    amw = new AndroidMainWindow ();

    pcmw = NULL;
    mmw = NULL;
    //FIXME
    mbw = new MobileBoostPidWindow (amw);
    mvis1w = new MobileVis1MainWindow (mbw);

    data = new MdData(mbw, mbw->ui->BoostGraphGroupBox, mvis1w, mvis1w->ui->Vis1PlotBox );

    mdcom = new MdBluetoothCom(this);
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


    //Replay
    replaySpeedUpFactor = 1;
    replay = new ReplayWorker(this);
    replayThread = new JobRunnerThread (this, replay);

    connect (replay, SIGNAL(clearPlots()), data, SLOT(clearPlots()) );

#if  !defined (Q_WS_MAEMO_5)  && !defined (Q_OS_ANDROID)
    connect (replay, SIGNAL(showStatusMessage(QString)), pcmw, SLOT(showStatusMessage(QString)), Qt::QueuedConnection );
#endif
    connect (replay, SIGNAL(visualizeDataRecord(MdDataRecord*,bool)), data, SLOT(visualizeDataRecord(MdDataRecord*,bool)), Qt::QueuedConnection );


#if  defined (Q_WS_MAEMO_5)  || defined (Q_OS_ANDROID)
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
    //	connect (pcmw->ui.actionShow_Boost_Lambda_Spectrogram, SIGNAL(triggered()), evalWinBoostLambdaSpectro, SLOT(show()) );

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
        mGps = NULL;
    if ( settings.value("mobile/use_accel", QVariant(true)).toBool() )
        accelMeter = new Accelerometer(this);
    else
        accelMeter = false;
#else
    mGps = NULL;
    accelMeter = NULL;
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
    connect (mds, SIGNAL(portClosed()), mvis1w, SLOT(enableReplay()));

    //V2 settings
    AndroidN75Dialog* an75 = new AndroidN75Dialog (amw, mds);
    connect (amw->ui->actionV2_N75_Settings, SIGNAL(triggered()), an75, SLOT(showMaximized()));

    connect (amw->ui->actionSettings, SIGNAL(triggered()), v2SettingsDialog, SLOT(showMaximized()));

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
    amw->ui->mainFrame->setContentsMargins(0,0,0,0);

    add = new AndroidDashboardDialog( );
    rtvis = new RealTimeVis ( add );
    connect (data, SIGNAL(rtNewDataRecord(MdDataRecord*)), rtvis, SLOT(visualize(MdDataRecord*)));

    if ( QAndroidJniObject::callStaticMethod<jboolean>( "de/gummelinformatics/mui/MuiIntentHelper", "hasPermanentMenuKey" ))
        connect (amw->ui->dashboardPushButton, SIGNAL(clicked()), add, SLOT(showFullScreen()) );
    else
        connect (amw->ui->dashboardPushButton, SIGNAL(clicked()), add, SLOT(showMaximized()) );

    connect (mdcom, SIGNAL(showStatusMessage(QString)), amw, SLOT(showStatusMessage(QString)) );
    connect (mds, SIGNAL(showStatusMessage(QString)), amw, SLOT(showStatusMessage(QString)) );
    connect (mdcom, SIGNAL(portClosed()), amw, SLOT(btPortClosed()) );
    connect (mdcom, SIGNAL(portOpened()), amw, SLOT(btPortOpened()) );

    connect (amw->ui->actionBluetoothToggleState, SIGNAL(triggered()), mdcom, SLOT(togglePort()) );
    connect (amw->ui->actionSave, SIGNAL(triggered()) , this, SLOT(saveDataAs()) );
    connect (amw->ui->actionOpen_Replay, SIGNAL(triggered()) , this, SLOT(openData()) );
    connect (amw->ui->actionClear, SIGNAL(triggered()) , this, SLOT(clearData()) );


    QSettings settings("MultiDisplay", "UI");
    if ( settings.value("mobile/use_gps", QVariant(true)).toBool() )
        mGps = new MobileGPS (this);
    else
        mGps = nullptr;
    if ( settings.value("mobile/use_accel", QVariant(true)).toBool() )
        accelMeter = new Accelerometer(this);
    else
        accelMeter = nullptr;

    connect ( v2SettingsDialog, SIGNAL(cfgDialogAccepted()), rtvis, SLOT(possibleCfgChange()) );

    //http://qt-project.org/doc/qt-5/qandroidjniobject.html#details
//    QAndroidJniObject activity = QtAndroid::androidActivity();
//    Q_ASSERT ( activity.isValid() );
//    activity.callMethod<void>("setKeepScreenOn", "(B)V", true);
#endif
}

void AppEngine::show() {
#if  !defined (Q_WS_MAEMO_5)  && !defined (Q_OS_ANDROID)
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
        add->showFullScreen();
    } else {
        amw->showMaximized();
        add->showMaximized();
    }
 #endif
}

void AppEngine::saveData () {
#if QT_VERSION >= 0x050000
    QString path =  QStandardPaths::standardLocations (QStandardPaths::DocumentsLocation)[0]
            + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".mdv2";
#else
    QString path = QDesktopServices::storageLocation (QDesktopServices::DocumentsLocation)
            + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".mdv2";
#endif

    data->saveData(path);
#if  defined (Q_WS_MAEMO_5)  || defined (Q_OS_ANDROID)
    if ( mGps ) {
        mGps->saveTrack (path + ".track");
        mGps->saveTrackBinary (path + ".trackB");
    }
#endif
}

void AppEngine::saveDataAsCSV() {
#if QT_VERSION >= 0x050000
    QString path =  QStandardPaths::standardLocations (QStandardPaths::DocumentsLocation)[0]
            + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".csv";
#else
    QString path = QDesktopServices::storageLocation (QDesktopServices::DocumentsLocation)
            + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".csv";
#endif
    QString fn = QFileDialog::getSaveFileName ( pcmw, QString("Select Filename"), path,
                                                "CSV (*.csv)");
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
    QAndroidJniObject s = QAndroidJniObject::callStaticObjectMethod( "de/gummelinformatics/mui/MuiIntentHelper", "getPublicDocumentPath", "()Ljava/lang/String;" );
    path = s.toString() + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".mdv2";
    qDebug() << "android save path 2019: " << path;
#endif

#else
    //Qt 4.x
    path = QDesktopServices::storageLocation (QDesktopServices::DocumentsLocation)
            + QDir::separator() + QDateTime::currentDateTime ().toString("yyyy-MM-ddThhmm") + ".mdv2";
#endif

    QString fn = QFileDialog::getSaveFileName ( pcmw, QString("Select Filename"), path,
                                                "mdv2 (*.mdv2)");
    if ( fn != "") {
        data->saveData(fn);
#if  defined (Q_WS_MAEMO_5)  || defined (ANDROID)
        if ( mGps ) {
            mGps->saveTrack (fn + ".track");
            mGps->saveTrackBinary (fn + ".trackB");
        }
#endif
    }
}
void AppEngine::openData ( QString fn ) {
    if ( fn == "" ) {
        //, QString("~"), QString("*.mdd")
//        QString path = QDesktopServices::storageLocation (QDesktopServices::DocumentsLocation);
        fn = QFileDialog::getOpenFileName ( pcmw, QString("Select Filename"), directory, "mdv2 (*.mdv2)" );
    }

    if ( fn != "") {
        directory = QFileInfo(fn).path(); // store path for next time
        qDebug() << "directory " << directory;

#ifdef Q_WS_MAEMO5
        mmw->setAttribute(Qt::WA_Maemo5ShowProgressIndicator);
#endif
        mds->closePort();

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

#if not defined Q_WS_MAEMO_5 and not defined Q_OS_ANDROID
        emit showStatusBarSampleCount( QString::number(data->size()) );
#endif

#if defined Q_OS_ANDROID
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

#if defined Q_OS_ANDROID
    if ( replay && ( replayThread->isRunning() || replayThread->isFinished() ) )
        replay->stop();
#endif
#if not defined Q_WS_MAEMO_5 and not defined Q_OS_ANDROID
    emit showStatusMessage( QString::number(data->size()) );
#endif
}

void AppEngine::changeSerialOptions() {
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

#if not defined Q_WS_MAEMO_5 and not defined Q_OS_ANDROID
    settings.beginGroup("MainWindow");
    settings.setValue("size", pcmw->size());
    settings.setValue("pos", pcmw->pos());
    settings.setValue("datatabindex", pcmw->ui.DataTableWidget->currentIndex());
    settings.endGroup();
#endif

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

#if  defined (Q_WS_MAEMO_5) || defined (Q_OS_ANDROID)
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

#if !defined(Q_WS_MAEMO_5) && !defined(ANDROID)
    settings.beginGroup("MainWindow");
    pcmw->resize(settings.value("size", QSize(800, 480)).toSize());
    pcmw->move(settings.value("pos", QPoint(200, 200)).toPoint());
    pcmw->ui.DataTableWidget->setCurrentIndex(settings.value ("datatabindex", 1).toInt());
    settings.endGroup();
#endif

    data->getVisPlot()->readSettings();


    mySerialOptionsDialog->getUi()->portComboBox->setCurrentIndex( settings.value ("mdserial/port", 0).toInt() );
    mySerialOptionsDialog->getUi()->speedComboBox->setCurrentIndex( settings.value ("mdserial/speed", 0).toInt() );
#if not defined ( Q_OS_ANDROID )
    mds->changePortSettings( mySerialOptionsDialog->getUi()->portComboBox->currentText(), mySerialOptionsDialog->getUi()->speedComboBox->currentText() );
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
#if defined Q_WS_MAEMO_5 and not defined Q_OS_ANDROID
    replaySpeedUpFactor = mvis1w->ui->ReplaySpinBox->value();
    replayStartAtPos = data->getVisPlot()->windowBegin();
#endif
#if not defined Q_WS_MAEMO_5 and not defined Q_OS_ANDROID
    if ( !pcmw->ui.ReplayCurPos->isChecked() )
        replayStartAtPos = 0;
    else
        replayStartAtPos = data->getVisPlot()->windowBegin();
#endif
//    replayThreadStopRequested = false;

    replayThread->start();

#if defined Q_WS_MAEMO_5 and not defined Q_OS_ANDROID
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

