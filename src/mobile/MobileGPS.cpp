#include "MobileGPS.h"

#include <QDebug>
#include <QGeoPositionInfoSource>
#include <QFile>
#include <QSettings>

#include "AppEngine.h"
#include "MdData.h"

MobileGPS::MobileGPS(QObject *parent)
    : QObject(parent), gpsUpdateCount(0)
{
    millisSinceLastGpsUpdate = 0;
    elapsedSinceLastMdFrame = 0;
    millisToNextMdFrame = 0;
    gpsSerial=NULL;

    QSettings settings("MultiDisplay", "UI");
    if ( settings.value("mobile/use_gps_internal", QVariant(true)).toBool() ) {
        qDebug() << "use N900 gps";

        //available only in qt mobility 1.2
        //N900 PR1.2 has 1.1 :(
    //    QString ss = "available positioning sources:";
    //    foreach ( QString s, QGeoPositionInfoSource::availableSources () ) {
    //        ss += " " + s;
    //    }
    //    qDebug() << ss;

        source = QGeoPositionInfoSource::createDefaultSource(this);
        if (source) {
            connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)),
                    this, SLOT(positionUpdated(QGeoPositionInfo)));
            //we want satellite
            source->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
            //minimal each 1000 millis!
            source->setUpdateInterval(1000);
            qDebug() << "MobileGPS updateIntervall=" << source->updateInterval()
                     << " min=" << source->minimumUpdateInterval();
            source->startUpdates();
        }
    } else {
        qDebug() << "use NMEA gps datastream on /dev/rfcomm5";
        gpsSerial = new MdGpsSerial();
        connect(gpsSerial, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdated(QGeoPositionInfo)));

    }
    freqMeasure = QTime::currentTime();
    deltaMdFrame = QTime::currentTime();
    freqMeasure.start();

}

void MobileGPS::mdFrameReceived() {
    millisToNextMdFrame = deltaMdFrame.restart();
}

void MobileGPS::positionUpdated(const QGeoPositionInfo &info) {
    elapsedSinceLastMdFrame = deltaMdFrame.restart();
    millisSinceLastGpsUpdate = freqMeasure.restart();
//    qDebug() << "milliselapsed " << millisSinceLastGpsUpdate;
    lastPositionInfo = info;
    gpsUpdateCount++;
    QGeoCoordinate newCoord = info.coordinate();
    if ( lastCoord != newCoord ) {
//        qDebug() << "Position updated:" << newCoord.toString();
        MdPos* e =  new MdPos();
        e->time = AppEngine::getInstance()->getData()->getLastTime();
        e->timeDelta = elapsedSinceLastMdFrame;
        e->pos = info;
        lastCoord = newCoord;
        track.append(e);
    }
}

bool MobileGPS::saveTrack(QString fn) {
    QFile file (fn);
    if ( !file.open (QIODevice::WriteOnly | QIODevice::Truncate) )
            return false;
    QTextStream ts (&file);
    ts << "#md timestamp\tgps gps-timedelta\ttime\tgps coordinate\tgps speed\tgps directon\tGPSHorizontalAccuracy\tGPSVerticalAccuracy" << endl;
    foreach (MdPos* e , track ) {
        if ( e->pos.isValid() ) {
            ts << e->time << '\t'
               << e->timeDelta << 't'
               << e->pos.timestamp().toString() << '\t'
               << e->pos.coordinate().toString() << '\t'
               << e->pos.attribute(QGeoPositionInfo::GroundSpeed) << '\t'
               << e->pos.attribute(QGeoPositionInfo::Direction) << '\t'
               << e->pos.attribute(QGeoPositionInfo::HorizontalAccuracy) << '\t'
               << e->pos.attribute(QGeoPositionInfo::VerticalAccuracy) << '\t'
               << endl;
        } else {
            qDebug() << "no valid position!" << endl;
        }
    }
    file.close();
    return true;
}
