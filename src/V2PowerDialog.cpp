#include "V2PowerDialog.h"
#include "ui_V2PowerDialog.h"

#include "PowerPlot.h"

V2PowerDialog::V2PowerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::V2PowerDialog)
{
    ui->setupUi(this);
    plot = new PowerPlot((QMainWindow*)this, (QWidget*) ui->groupBox);

    connect (ui->pressureSpinBox, SIGNAL(valueChanged(int)), plot, SLOT(setDinAirPressure(int)));
    connect (ui->tempSpinBox, SIGNAL(valueChanged(int)), plot, SLOT(setDinTemp(int)));
    connect (ui->carMassSpinBox, SIGNAL(valueChanged(int)), plot, SLOT(setCarMass(int)));
    connect (ui->smoothSpinBox, SIGNAL(valueChanged(int)), plot, SLOT(setSmoothAmount(int)));
    connect (ui->driveTrainLossSpinBox, SIGNAL(valueChanged(double)), plot, SLOT(setDriveTrainLoss(double)));

    connect (ui->reCalcButton, SIGNAL(clicked()), plot, SLOT(reCalculate()) );

    connect (plot, SIGNAL(resultString(QString)), ui->resultLineEdit, SLOT(setText(QString)));

    ui->pressureSpinBox->setValue(plot->dinAirPressure());
    ui->tempSpinBox->setValue(plot->dinTemp());
    ui->carMassSpinBox->setValue(plot->getCarMass());
    ui->smoothSpinBox->setValue(plot->getSmoothAmount());
    ui->driveTrainLossSpinBox->setValue(plot->getDriveTrainLoss());
}

V2PowerDialog::~V2PowerDialog()
{
    delete ui;
}
