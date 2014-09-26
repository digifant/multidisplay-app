#ifndef V2POWERDIALOG_H
#define V2POWERDIALOG_H

#include <QDialog>

namespace Ui {
class V2PowerDialog;
}

#include "PowerPlot.h"

class V2PowerDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit V2PowerDialog(QWidget *parent = 0);
    ~V2PowerDialog();

    PowerPlot* powerPlot () { return plot; };
    
private:
    Ui::V2PowerDialog *ui;
    PowerPlot *plot;
};

#endif // V2POWERDIALOG_H
