#ifndef GEARSETTINGSDIALOG_H
#define GEARSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class GearSettingsDialog;
}

class GearSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GearSettingsDialog(QWidget *parent = 0);
    ~GearSettingsDialog();

signals:
    void writeGearboxData (double g1, double g2, double g3, double g4, double g5, double g6, quint8);
    void requestGearboxData ();

protected slots:
    void loadDefaultTransmission ( const QString & text );
    void loadMdGearboxData(quint8, double g1, double g2, double g3, double g4, double g5, double g6);
    void myacceptedSlot();

protected:
    virtual void showEvent ( QShowEvent * event );

private:
    Ui::GearSettingsDialog *ui;
};

#endif // GEARSETTINGSDIALOG_H
