#ifndef MOBILEGPS_H
#define MOBILEGPS_H

#include <QObject>
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>
#include <QGeoCoordinate>


#include "MdGpsSerial.h"

#if defined Q_WS_MAEMO_5
QTM_USE_NAMESPACE
#endif

struct MdPos {
    int time;
    int timeDelta;
    QGeoPositionInfo pos;
};
QDataStream& operator<< (QDataStream& s, MdPos *p);
QDataStream& operator>> (QDataStream& s, MdPos *p);

class MobileGPS : public QObject
{
    Q_OBJECT

public:
    MobileGPS(QObject *parent = 0);

    enum { MAGICNUMBER = 0xFEABFEAB, VERSION = 1 };

    bool saveTrack (QString fn);
    bool saveTrackBinary (QString fn);
    //! broken
    bool loadTrack (QString fn);
    bool loadTrackBinary (QString fn);
    void clearData();

    QGeoPositionInfo& lastPos() { return lastPositionInfo; };
    quint32 updateCount () { return gpsUpdateCount; };

    int millisSinceLastGpsUpdate;
    int elapsedSinceLastMdFrame;
    int millisToNextMdFrame;


public slots:
    void positionUpdated(const QGeoPositionInfo &info);
    void mdFrameReceived();

protected:

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
