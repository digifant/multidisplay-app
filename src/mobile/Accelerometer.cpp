#include "Accelerometer.h"

#include <QDebug>

Accelerometer::Accelerometer(QObject *parent) :
    QObject(parent), x(0), y(0), z(0), milliselapsed(0)
{
    //http://doc.qt.digia.com/qtmobility-1.2/qaccelerometerreading.html
    sensor = new QAccelerometer(this);
    qDebug() << "Accelerometer datarate=" << sensor->dataRate();

    //empty
//    qDebug() << "availableDataRates()";
//    for ( QList< QPair<int,int> >::iterator it = sensor->availableDataRates().begin() ; it != sensor->availableDataRates().end() ; ++it ) {
//        qDebug() << (*it).first << " | " << (*it).second;
//    }
    //setDataRate: rate 10 is not supported by the sensor.
    //sensor->setDataRate(10);
    connect(sensor, SIGNAL(readingChanged()), this, SLOT(checkReading()));
    sensor->start();
    freqMeasure = QTime::currentTime();
    freqMeasure.start();
}


void Accelerometer::checkReading() {
    milliselapsed = freqMeasure.restart();
//    qDebug() << "accel reading: X=" << sensor->reading()->x() << " Y=" << sensor->reading()->y()
//             << " Z=" << sensor->reading()->z() << " t=" << milliselapsed;
    x = sensor->reading()->x();
    y = sensor->reading()->y();
    z = sensor->reading()->z();
    // the N900 does not do this. Instead, it orients the hardware sensors towards its default landscape orientation.
    // N900 landscape mode: x left / right, y up/down, z=front/back
}
