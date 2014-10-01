#ifndef MOBILEGPS_H
#define MOBILEGPS_H

#include <QObject>
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>
#include <QGeoCoordinate>


#include "MdGpsSerial.h"

#if defined Q_WS_MAEMO5
QTM_USE_NAMESPACE
#endif

struct MdPos {
    int time;
    int timeDelta;
    QGeoPositionInfo pos;
};

class MobileGPS : public QObject
{
    Q_OBJECT

public:
    MobileGPS(QObject *parent = 0);

    bool saveTrack (QString fn);

    QGeoPositionInfo& lastPos() { return lastPositionInfo; };
    quint32 updateCount () { return gpsUpdateCount; };

    int millisSinceLastGpsUpdate;
    int elapsedSinceLastMdFrame;
    int millisToNextMdFrame;


public slots:
    void positionUpdated(const QGeoPositionInfo &info);
    void mdFrameReceived();

private:
    QGeoPositionInfoSource *source;
    QGeoPositionInfo lastPositionInfo;
    QGeoCoordinate lastCoord;
    QList<MdPos*> track;
    quint32 gpsUpdateCount;
    QTime freqMeasure;
    QTime deltaMdFrame;
    MdGpsSerial *gpsSerial;
};

#endif // MOBILEGPS_H
