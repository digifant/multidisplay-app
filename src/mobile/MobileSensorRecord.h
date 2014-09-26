#ifndef MOBILESENSORRECORD_H
#define MOBILESENSORRECORD_H

#include <QDateTime>
#include <QtGlobal>
#include <QDataStream>

class MobileSensorRecord
{
    friend QDataStream& operator<<(QDataStream&, MobileSensorRecord*);
    friend QDataStream& operator>>(QDataStream&, MobileSensorRecord*);
public:
    MobileSensorRecord();
    MobileSensorRecord(qreal &accX, qreal &accY, qreal &accZ, int &mdTimestamp, QDateTime &gpsTimestamp,
                       QString &gpsCoordinateString, qreal &gpsAltitude, qreal &gpsGroundSpeed, qreal &gpsDirection,
                       qreal &gpsHorizontalAccuracy, qreal &gpsVerticalAccuracy, bool &gpsValid, quint32 gpsUpdateCount);

    QVariant getColumn ( const int & column );


    double accX;
    double accY;
    double accZ;

    int mdTimestamp;
    QDateTime	gpsTimestamp;
    QString gpsCoordinateString;
    double gpsAltitude;
    double gpsGroundSpeed;
    double gpsDirection;
    double gpsHorizontalAccuracy;
    double gpsVerticalAccuracy;
    bool gpsValid;

    quint32 gpsUpdateCount;
    int millisElapsedSinceLastMdFrame;
};

QDataStream& operator<< (QDataStream& s, MobileSensorRecord* b);
QDataStream& operator>> (QDataStream& s, MobileSensorRecord* b);

#endif // MOBILESENSORRECORD_H
