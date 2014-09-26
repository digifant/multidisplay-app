#include <QtMaemo5/QMaemo5ListPickSelector>
#include <QtMaemo5/QMaemo5ValueButton>
#include <QStandardItemModel>

#include "MobileEvaluationDialog.h"
#include "ui_MobileEvaluationDialog.h"

#include "AppEngine.h"
#include "evaluation/evaluationwindow.h"

MobileEvaluationDialog::MobileEvaluationDialog(QWidget *parent, EvaluationWindow *evalWinBoostLambda,
                                               EvaluationWindow *evalWinRPMBoost) :
    QDialog(parent),
    ui(new Ui::MobileEvaluationDialog)
{
    ui->setupUi(this);

    QStandardItemModel *model = new QStandardItemModel (2,1, this);
    model->setItem(0, new QStandardItem("Boost"));
    model->setItem(1, new QStandardItem("EGT"));

    QMaemo5ValueButton *maxValButton = new QMaemo5ValueButton("show max values");
    maxValButton->setValueLayout(QMaemo5ValueButton::ValueBesideText);
    QMaemo5ListPickSelector *selector1 = new QMaemo5ListPickSelector();
    selector1->setModel(model);
    // not setting the current index means that the value is empty.
    maxValButton->setPickSelector(selector1);

    ui->gridLayout->addWidget(maxValButton, 1, 2, 1, 2);

    if ( evalWinBoostLambda )
        connect (ui->evalBoostLambdaButton, SIGNAL(clicked()), evalWinBoostLambda, SLOT(show()) );
    if ( evalWinRPMBoost )
        connect (ui->evalRpmBoostButton, SIGNAL(clicked()), evalWinRPMBoost, SLOT(show()) );
    //	connect (pcmw->ui.actionShow_Boost_Lambda_Spectrogram, SIGNAL(triggered()), evalWinBoostLambdaSpectro, SLOT(show()) );

}

MobileEvaluationDialog::~MobileEvaluationDialog()
{
    delete ui;
}

