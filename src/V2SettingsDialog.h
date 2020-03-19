#ifndef V2SETTINGSDIALOG_H
#define V2SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class V2SettingsDialog;
}

class V2SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit V2SettingsDialog(QWidget *parent = 0);
    ~V2SettingsDialog();

signals:
    void cfgDialogAccepted();

protected slots:
    void save();
    void signalFinished(int);
    void signalRejected();

protected:
    void showEvent ( QShowEvent * event );

private:
    Ui::V2SettingsDialog *ui;
};

#endif // V2SETTINGSDIALOG_H
