#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <QObject>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QTime>
#else
#include <QElapsedTimer>
#endif

#include <QAccelerometer>

#if defined Q_WS_MAEMO_5
QTM_USE_NAMESPACE
#endif

class Accelerometer : public QObject
{
    Q_OBJECT
public:
    explicit Accelerometer(QObject *parent = 0);
    
    QAccelerometerReading *reading() { return sensor->reading(); };

    qreal x;
    qreal y;
    qreal z;

    int milliselapsed;

signals:
    
public slots:
    void checkReading();

private:
    QAccelerometer* sensor;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QTime freqMeasure;
#else
    QElapsedTimer freqMeasure;
#endif
    //current readings
};

#endif // ACCELEROMETER_H
