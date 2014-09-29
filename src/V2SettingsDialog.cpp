#include "V2SettingsDialog.h"

#include <QDebug>
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
}

void V2SettingsDialog::showEvent ( QShowEvent * event ) {
    Q_UNUSED(event);
    ui->actualizeVis1CheckBox->setChecked( AppEngine::getInstance()->getActualizeVis1() );
    ui->actualizeDashboardCheckBox->setChecked( AppEngine::getInstance()->getActualizeDashboard() );
}
