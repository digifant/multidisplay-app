#ifndef N75OPTIONSDIALOG_H
#define N75OPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
    class N75OptionsDialog;
}

class N75OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit N75OptionsDialog(QWidget *parent = 0);
    ~N75OptionsDialog();

    Ui::N75OptionsDialog *ui;
};

#endif // N75OPTIONSDIALOG_H
