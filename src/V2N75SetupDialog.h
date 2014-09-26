#ifndef V2N75SETUPDIALOG_H
#define V2N75SETUPDIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
    class V2N75SetupDialog;
}

class N75TableWidget;
class N75PidSettingsWidget;

class V2N75SetupDialog : public QDialog
{
    Q_OBJECT

    friend class AppEngine;
public:
    explicit V2N75SetupDialog(QWidget *parent = 0);
    ~V2N75SetupDialog();

signals:
    void n75reqDutyMap (quint8 gear, quint8 mode, quint8 serial);
    void n75reqSetpointMap (quint8 gear, quint8 mode, quint8 serial);
    void n75writeDutyMap (quint8 gear, quint8 mode, quint8 serial, QVector<quint8>* data);
    void n75writeSetpointMap (quint8 gear, quint8 mode, quint8 serial, QVector<double>* data);

public slots:
    void n75modeChanged ( int index );
    void n75read ();
    void n75readSlow ();
    void n75write ();
    void n75writeSlow ();
    void n75dutyMap (quint8 gear, quint8 mode, quint8 serial, QVector<quint8> *data);
    void n75SetpointMap (quint8 gear, quint8 mode, quint8 serial, QVector<double> *data);

    void ackReceived (quint8 serial);

    void timerUpdateRead();
    void timerUpdateWrite();

protected:
    void showEvent ( QShowEvent * event );
    void closeEvent ( QCloseEvent * event );

private:
    Ui::V2N75SetupDialog *ui;
    N75TableWidget *n75lowTw;
    N75TableWidget *n75highTw;
    N75PidSettingsWidget *n75Settings;
    QTimer *t;
    QTimer *wt;

    quint8 next_gear;
    quint8 next_setpoint_or_gear;
    bool wait_for_n75_map_frame;
    quint8 next_gear_write;
    quint8 next_setpoint_or_gear_write;
    quint8 write_wait_for_ack_serial;

    bool dashboardActualizeSave;
    bool vis1ActualizeSave;
};

#endif // V2N75SETUPDIALOG_H
