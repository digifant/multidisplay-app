#include "V2SettingsDialog.h"

#include <QDebug>
#include <QSettings>
#include "ui_V2SettingsDialog.h"
#include "AppEngine.h"
#include "com/MdBinaryProtocol.h"

V2SettingsDialog::V2SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::V2SettingsDialog)
{
    ui->setupUi(this);
#ifdef Q_OS_ANDROID
    //we want no ok / cancel button on android. data is saved on close by back button
    //back button triggers 1. rejected and 2. finished
    connect (this, SIGNAL(rejected()), this, SLOT(accept() ));
    //connect (this, SIGNAL(finished(int)), this, SLOT(signalFinished(int) ));
    connect (this, SIGNAL(accepted()), this, SLOT(save()));
    ui->buttonBox->hide();
    //delete on close! AndroidMainWindows eventhandler will initiate the re-creation
    //fixes broken dialog on 2. show
    setAttribute( Qt::WA_DeleteOnClose, true );
#else
    connect (ui->buttonBox, SIGNAL(accepted()), this, SLOT(save()));
#endif
}

V2SettingsDialog::~V2SettingsDialog()
{
    delete ui;
}

void V2SettingsDialog::save() {
    AppEngine::getInstance()->setActualizeVis1( ui->actualizeVis1CheckBox->isChecked() );
    AppEngine::getInstance()->setActualizeDashboard( ui->actualizeDashboardCheckBox->isChecked() );

    MdBinaryProtocol* mds = qobject_cast<MdBinaryProtocol*> (AppEngine::getInstance()->getMdBinaryProtocl());
    if ( mds )
        mds->mdCmdSetSerialFrequency (ui->serialFrequencySpinBox->value(), 0);
    else
        qDebug() << "NULL";

    QSettings settings;
    settings.setValue ("md/ecu", QVariant (ui->comboBoxEcu->currentText()) );

    if ( ui->comboBoxMapSensor->currentText().toInt() != settings.value ("Digifant1/MapSensor", 200) ) {
        int newDfMap = ui->comboBoxMapSensor->currentText().toInt();
        settings.setValue("Digifant1/MapSensor", newDfMap);
        switch (newDfMap) {
        case 400 :
            AppEngine::getInstance()->setDfBoostTransferFunction( new TransferFunction400kpa() );
            break;
        case 250 :
            AppEngine::getInstance()->setDfBoostTransferFunction( new TransferFunction250kpa() );
            break;
        case 100 :
            AppEngine::getInstance()->setDfBoostTransferFunction( new TransferFunction100kpa() );
            break;
        case 300 :
            AppEngine::getInstance()->setDfBoostTransferFunction( new TransferFunction300kpa() );
            break;
        case 200 :
        default:
            AppEngine::getInstance()->setDfBoostTransferFunction( new TransferFunction200kpa() );
            break;
        }
        //in setDfBoostTransferFunction
        //emit newDfBoostTransferFunction (dfBoostTransferFunction->name());
    }

    emit cfgDialogAccepted();
}

void V2SettingsDialog::signalFinished(int result)
{
    //android: back button triggers 2. signal finished
}

void V2SettingsDialog::signalRejected()
{
    //android: back button triggers 1. signal rejected
    //qt: cancel triggers signal rejected

}

void V2SettingsDialog::showEvent ( QShowEvent * event ) {
    Q_UNUSED(event);
    ui->actualizeVis1CheckBox->setChecked( AppEngine::getInstance()->getActualizeVis1() );
    ui->actualizeDashboardCheckBox->setChecked( AppEngine::getInstance()->getActualizeDashboard() );
    QSettings settings;
    QString ecuStr = settings.value("md/ecu", QVariant (QString("Digifant 1"))).toString();
    bool found = false;
    for (uint i = 0 ; i < ui->comboBoxEcu->count() ; ++i) {
        if ( ui->comboBoxEcu->itemText(i) == ecuStr ) {
            ui->comboBoxEcu->setCurrentIndex(i);
            found = true;
        }
    }
    if ( !found) {
        ui->comboBoxEcu->insertItem(0, ecuStr);
        ui->comboBoxEcu->setCurrentIndex(0);
    }

    int dfMap = settings.value ("Digifant1/MapSensor", 200).toInt();
    for ( int i = 0 ; i < ui->comboBoxMapSensor->count() ; i++ ) {
        if ( ui->comboBoxMapSensor->itemText(i).toInt() == dfMap ) {
            ui->comboBoxMapSensor->setCurrentIndex(i);
        }
    }
#if defined ( Q_OS_ANDROID)
    {
        ui->groupBoxFrequency->hide();
        ui->groupBoxEnergy->hide();
        //FIX android dark holo theme default has light background and text color! -> set background to black!
        QColor fr = ui->comboBoxEcu->palette().color(QWidget::foregroundRole());
        //light 0 dark 243
        //qDebug() << fr.lightness();
        if ( fr.lightness() >= 243 ) {
            ui->comboBoxEcu->setStyleSheet("QComboBox { background-color: black }" "QListView { color: blue; }");
            //ui->comboBoxWbLambda->setStyleSheet("QComboBox { background-color: black }" "QListView { color: blue; }");
            ui->comboBoxMapSensor->setStyleSheet("QComboBox { background-color: black }" "QListView { color: blue; }");
            //ui->buttonBox->setStyleSheet("QPushButton { background-color: black }");
        }
    }
#endif
}
