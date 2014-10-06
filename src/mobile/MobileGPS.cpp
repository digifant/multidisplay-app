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
        qDebug() << "using internal gps";

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
#if defined (Q_WS_MAEMO_5)
        qDebug() << "use NMEA gps datastream on /dev/rfcomm5";
        gpsSerial = new MdGpsSerial();
        connect(gpsSerial, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdated(QGeoPositionInfo)));
#endif
#if defined (Q_OS_ANDROID)
        qDebug() << "use NMEA gps datastream on bluetooth spp";
        gpsSerial = new MdGpsSerial();
        connect(gpsSerial, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdated(QGeoPositionInfo)));
#endif

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
    ts << "#md timestamp\ttimedelta\tgps time\tgps coordinate\tgps speed\tgps directon\tGPSHorizontalAccuracy\tGPSVerticalAccuracy" << endl;
    foreach (MdPos* e , track ) {
        if ( e->pos.isValid() ) {
            ts << e->time << '\t'
               << e->timeDelta << '\t'
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

bool MobileGPS::saveTrackBinary(QString fn)
{
    QFile file (fn);
    if ( !file.open (QIODevice::WriteOnly | QIODevice::Truncate) )
            return false;
    QDataStream ds (&file);
    ds.setVersion(QDataStream::Qt_4_7);
    ds << MAGICNUMBER;
    ds << VERSION;

    foreach ( MdPos* p, track ) {
        ds << p;
    }
    file.close();
    return true;
}

bool MobileGPS::loadTrack(QString fn)
{
    QFile file (fn);
    if ( !file.open (QIODevice::ReadOnly) )
        return false;
    QTextStream ts (&file);
    QString line;
    do {
        line = ts.readLine();
        if ( !line.isNull() ) {
            MdPos* pos = new MdPos ();
            QStringList l = line.split('\t');
            pos->time = l[0].toInt();
            pos->timeDelta = l[1].toInt();
            QString degreesMinutesSecondsWithHemisphereOptionalHeigth = l[3];
            QStringList dmsHs = degreesMinutesSecondsWithHemisphereOptionalHeigth.split(",");
            //            51° 5' 60.0" N, 10° 29' 60.0" E, 211.5m
            QRegExp dmshR ("(\\d+)\\D\\s*(\\d+)\\D\\s*(\\d+)\\D\\s*(\\w)");
            QRegExp heigthR ("(\\d+\\.\\d+)m");

            QGeoPositionInfo pi;
            pi.setTimestamp( QDateTime::fromString( QString(line[2])) );

            //TODO fixme

//            pi.setCoordinate( QGeoCoordinate() );
            track.append(pos);
        }
    } while (!line.isNull());
    file.close();
}

bool MobileGPS::loadTrackBinary(QString fn)
{
    clearData();

    QFile file (fn);
        if ( !file.open (QIODevice::ReadOnly) )
            return false;

    QDataStream ds (&file);
    ds.setVersion(QDataStream::Qt_4_7);
    quint32 magic, version;
    ds >> magic;
    ds >> version;

    if ( (magic == MAGICNUMBER) && (version==VERSION) ) {
        while ( !ds.atEnd() ) {
            MdPos *p = new MdPos();
            ds >> p;
            track.append(p);
        }
        return true;
    } else {
        return false;
    }
}

void MobileGPS::clearData()
{
    foreach (MdPos* p , track ) {
        if ( p )
            delete (p);
        track.clear();
    }
}


QDataStream &operator<<(QDataStream &s, MdPos *p)
{
    s << p->time;
    s << p->timeDelta;
    s << p->pos;
    return s;
}

QDataStream &operator>>(QDataStream &s, MdPos *p)
{
    s >> p->time;
    s >> p->timeDelta;
    s >> p->pos;
    return s;
}


