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
    connect (ui->buttonBox, SIGNAL(accepted()), this, SLOT(accepted()));
}

V2SettingsDialog::~V2SettingsDialog()
{
    delete ui;
}

void V2SettingsDialog::accepted() {
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
    }
#endif
}
