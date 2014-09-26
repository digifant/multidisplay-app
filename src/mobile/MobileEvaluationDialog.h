#ifndef MOBILEEVALUATIONDIALOG_H
#define MOBILEEVALUATIONDIALOG_H

#include <QDialog>

namespace Ui {
class MobileEvaluationDialog;
}

class EvaluationWindow;

class MobileEvaluationDialog : public QDialog
{
    Q_OBJECT
    
    friend class AppEngine;

public:
    explicit MobileEvaluationDialog(QWidget *parent = 0, EvaluationWindow *evalWinBoostLambda=0,
                                    EvaluationWindow *evalWinRPMBoost=0);
    ~MobileEvaluationDialog();
    
private:
    Ui::MobileEvaluationDialog *ui;
};

#endif // MOBILEEVALUATIONDIALOG_H
