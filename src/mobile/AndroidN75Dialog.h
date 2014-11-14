#ifndef ANDROIDN75DIALOG_H
#define ANDROIDN75DIALOG_H

#include <QDialog>

namespace Ui {
class AndroidN75Dialog;
}

class QSwipeGesture;
class QGestureEvent;
class N75TableWidget;
class N75PidSettingsWidget;
class QTimer;

class AndroidN75Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit AndroidN75Dialog(QWidget *parent = 0);
    ~AndroidN75Dialog();

signals:
    void n75reqDutyMap (quint8 gear, quint8 mode, quint8 serial);
    void n75reqSetpointMap (quint8 gear, quint8 mode, quint8 serial);
    void n75writeDutyMap (quint8 gear, quint8 mode, quint8 serial, QVector<quint8>* data);
    void n75writeSetpointMap (quint8 gear, quint8 mode, quint8 serial, QVector<double>* data);

public slots:
//    void n75read ();
    void n75readSlow (int idx=-1);
//    void n75write ();
    void n75writeSlow (int idx=-1);
    void n75dutyMap (quint8 gear, quint8 mode, quint8 serial, QVector<quint8> *data);
    void n75SetpointMap (quint8 gear, quint8 mode, quint8 serial, QVector<double> *data);

    void ackReceived (quint8 serial);

    void timerUpdateRead();
    void timerUpdateWrite();

protected:
    void resizeEvent ( QResizeEvent * event );
    bool event(QEvent *event);
    bool swipeTriggered(QSwipeGesture *pSwipe);
    bool gestureEvent(QGestureEvent *event);
    void showEvent ( QShowEvent * event );
    void closeEvent ( QCloseEvent * event );

private:
    Ui::AndroidN75Dialog *ui;

    bool landscape;

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
    int n75_idx;

    int m_gestureId;
};

#endif // ANDROIDN75DIALOG_H
