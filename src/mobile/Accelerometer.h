#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <QObject>
#include <QTime>

#include <QAccelerometer>

QTM_USE_NAMESPACE

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
    QTime freqMeasure;
    //current readings
};

#endif // ACCELEROMETER_H
