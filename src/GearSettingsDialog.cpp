#include "GearSettingsDialog.h"
#include "ui_GearSettingsDialog.h"
#include "AppEngine.h"
#include "com/MdBinaryProtocol.h"

#include <QDebug>

GearSettingsDialog::GearSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GearSettingsDialog)
{
    ui->setupUi(this);
    connect (ui->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(loadDefaultTransmission(QString)));
    connect (this, SIGNAL(accepted()), this, SLOT(myacceptedSlot()));
}

GearSettingsDialog::~GearSettingsDialog()
{
    delete ui;
}

void GearSettingsDialog::showEvent ( QShowEvent * event ) {
    qDebug() << "GearSettingsDialog::showEvent()";
    MdBinaryProtocol* mds = qobject_cast<MdBinaryProtocol*> (AppEngine::getInstance()->getMdBinaryProtocl());
    connect ( mds, SIGNAL(gearboxSettingsReceived(quint8,double,double,double,double,double,double)),
             this, SLOT(loadMdGearboxData(quint8,double,double,double,double,double,double)));
    connect (this, SIGNAL(writeGearboxData(double,double,double,double,double,double,quint8)),
             mds, SLOT(mdCmdWriteGearbox(double,double,double,double,double,double,quint8)));
    mds->mdCmdReadGearbox(99);
}

void GearSettingsDialog::loadMdGearboxData(quint8 serial, double g1, double g2, double g3, double g4, double g5, double g6){
    qDebug() << "GearSettingsDialog::loadMdGearboxData serial=" << serial;
    ui->g1SpinBox->setValue(g1);
    ui->g2SpinBox->setValue(g2);
    ui->g3SpinBox->setValue(g3);
    ui->g4SpinBox->setValue(g4);
    ui->g5SpinBox->setValue(g5);
    ui->g6SpinBox->setValue(g6);
}

void GearSettingsDialog::myacceptedSlot() {
    qDebug() << "GearSettingsDialog::myacceptedSlot()";

    emit ( writeGearboxData(ui->g1SpinBox->value(), ui->g2SpinBox->value(), ui->g3SpinBox->value(),
                            ui->g4SpinBox->value(), ui->g5SpinBox->value(), ui->g6SpinBox->value(),
                            99) );
}

void GearSettingsDialog::loadDefaultTransmission ( const QString & text ) {
    double gear_ratio[6];

    if ( text == "02A ATB" ) {
        //Corrado 02A ATB
        gear_ratio[0] = 13.0341;
        gear_ratio[1] = 7.26225;
        gear_ratio[2] = 4.64025;
        gear_ratio[3] = 3.34995;
        gear_ratio[4] = 2.74275;
        gear_ratio[5] = 2.74275;
    }
    if ( text == "02A AYN" ) {
        //Corrado AYN
        gear_ratio[0] = 13.918152;
        gear_ratio[1] = 7.802712;
        gear_ratio[2] = 4.95498;
        gear_ratio[3] = 3.577164;
        gear_ratio[4] = 2.785;
        gear_ratio[5] = 2.785;
    }
    if ( text == "02A CBA" ) {
        //Corrado CBA
        gear_ratio[0] = 13.816146;
        gear_ratio[1] = 7.745526;
        gear_ratio[2] = 4.97352;
        gear_ratio[3] = 3.536319;
        gear_ratio[4] = 2.812233;
        gear_ratio[5] = 2.812233;
    }
    if ( text == "02M FMP custom" ) {
        //R32 FMP custom
        gear_ratio[0] = 10.870;
        gear_ratio[1] = 6.758;
        gear_ratio[2] = 4.757;
        gear_ratio[3] = 3.555;
        gear_ratio[4] = 2.897;
        gear_ratio[5] = 2.424;
    }
    if ( text == "02A ATB custom with fifth gear from ASD" ) {
        //Corrado 02A ATB with 5. gear from ASD (diesel)
        gear_ratio[0] = 13.0341;
        gear_ratio[1] = 7.26225;
        gear_ratio[2] = 4.64025;
        gear_ratio[3] = 3.34995;
        gear_ratio[4] = 2.4702;
        gear_ratio[5] = 2.4702;
    }
    if ( text == "02A CCM (VR6)" ) {
         //Golf 3 CCM (VR6)
         gear_ratio[0] = 11.1837;
         gear_ratio[1] = 6.588216;
         gear_ratio[2] = 4.432812;
         gear_ratio[3] = 3.504226;
         gear_ratio[4] = 2.839982;
         gear_ratio[5] = 2.839982;
    }
    if ( text == "02A CSR (VR6 Syncro)") {
        //Golf 3 CSR (VR6 Syncro)
        gear_ratio[0] = 12.2525;
        gear_ratio[1] = 6.99998;
        gear_ratio[2] = 4.97874;
        gear_ratio[3] = 3.50844;
        gear_ratio[4] = 2.84934;
        gear_ratio[5] = 2.84934;
    }
    if ( text == "02M DQB") {
        //S3
        gear_ratio[0] = 14.364;
        gear_ratio[1] = 8.862;
        gear_ratio[2] = 6.0018;
        gear_ratio[3] = 4.5696;
        gear_ratio[4] = 3.6373;
        gear_ratio[5] = 3.023;
    }
    if ( text == "02A CDA") {
        //Golf3 16V (Achsuebersetzung  3,684
        gear_ratio[0] = 11.052;
        gear_ratio[1] = 7.1617;
        gear_ratio[2] = 4.8187;
        gear_ratio[3] = 3.7907;
        gear_ratio[4] = 3.0834;
        gear_ratio[5] = 3.0834;
    }

    ui->g1SpinBox->setValue( gear_ratio[0] );
    ui->g2SpinBox->setValue( gear_ratio[1] );
    ui->g3SpinBox->setValue( gear_ratio[2] );
    ui->g4SpinBox->setValue( gear_ratio[3] );
    ui->g5SpinBox->setValue( gear_ratio[4] );
    ui->g6SpinBox->setValue( gear_ratio[5] );
}
