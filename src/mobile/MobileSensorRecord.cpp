#include "MobileSensorRecord.h"

#include <QVariant>

MobileSensorRecord::MobileSensorRecord()
    : accX(0), accY(0), accZ(0), mdTimestamp(0), gpsAltitude(0), gpsGroundSpeed(0),
      gpsDirection(0), gpsHorizontalAccuracy(0), gpsVerticalAccuracy(0), gpsValid(false)
{
}

MobileSensorRecord::MobileSensorRecord(qreal &accX, qreal &accY, qreal &accZ, int &mdTimestamp,
                                       QDateTime &gpsTimestamp, QString &gpsCoordinateString,
                                       qreal &gpsAltitude, qreal &gpsGroundSpeed, qreal &gpsDirection,
                                       qreal &gpsHorizontalAccuracy, qreal &gpsVerticalAccuracy,
                                       bool &gpsValid, quint32 gpsUpdateCount)
    : accX(accX), accY(accY), accZ(accZ), mdTimestamp(mdTimestamp), gpsTimestamp(gpsTimestamp),
      gpsCoordinateString(gpsCoordinateString), gpsAltitude(gpsAltitude), gpsGroundSpeed(gpsGroundSpeed),
      gpsDirection(gpsDirection), gpsHorizontalAccuracy(gpsHorizontalAccuracy),
      gpsVerticalAccuracy(gpsVerticalAccuracy),
      gpsValid(gpsValid), gpsUpdateCount(gpsUpdateCount)
{
}

QVariant MobileSensorRecord::getColumn ( const int & column ) {
    switch ( column ) {
    case 48:
        return gpsCoordinateString;
        break;
    case 49:
        return gpsGroundSpeed;
        break;
    case 50:
        return gpsAltitude;
        break;
    case 51:
        return QString ( QString::number(accX, 'f', 2) + " | "
                         + QString::number(accY,'f',2) + " | "
                         + QString::number(accZ, 'f',2) );
        break;
    }
    return QVariant();
}

QDataStream& operator<< (QDataStream& s, MobileSensorRecord* b) {
    s << b->accX;
    s << b->accY;
    s << b->accZ;
    s << b->mdTimestamp;
    s << b->gpsTimestamp;
    s << b->gpsCoordinateString;
    s << b->gpsAltitude;
    s << b->gpsGroundSpeed;
    s << b->gpsDirection;
    s << b->gpsHorizontalAccuracy;
    s << b->gpsVerticalAccuracy;
    s << b->gpsValid;
    s << b->gpsUpdateCount;
    s << b->millisElapsedSinceLastMdFrame;
    return s;
}

QDataStream& operator>> (QDataStream& s, MobileSensorRecord* b) {
    s >> b->accX;
    s >> b->accY;
    s >> b->accZ;
    s >> b->mdTimestamp;
    s >> b->gpsTimestamp;
    s >> b->gpsCoordinateString;
    s >> b->gpsAltitude;
    s >> b->gpsGroundSpeed;
    s >> b->gpsDirection;
    s >> b->gpsHorizontalAccuracy;
    s >> b->gpsVerticalAccuracy;
//    double d;
//    s >> d;
//    b->gpsUpdateCount = d;
//    b->gpsVerticalAccuracy=0;

    s >> b->gpsValid;
    s >> b->gpsUpdateCount;
//    int t;
//    s >> t;

    s >> b->millisElapsedSinceLastMdFrame;
//    b->millisElapsedSinceLastMdFrame=0;

    return s;
}
