#include "MdGpsSerial.h"

#include <QTime>
#include <QDebug>
#include <QStringList>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <com/MdQSerialPortCom.h>
    #include <com/MdBluetoothCom.h>
#else
    #include <com/MdQextSerialCom.h>
#endif


MdGpsSerial::MdGpsSerial()
{
    setupPort();
}

MdGpsSerial::~MdGpsSerial() {
    if ( port )
        delete (port);
}

bool MdGpsSerial::setupPort (QString sport, QString speed) {
    if ( port )
        delete port;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) and defined Q_OS_ANDROID
    port = new MdBluetoothCom(this);
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) and !defined Q_OS_ANDROID
    port = new MdQSerialPortCom(this);
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    port = new MdQextSerialCom(this);
#endif

    bool r = port->setupPort (sport,speed);
    connect(port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    if ( r )
        qDebug() << "MdGpsSerial: listening for data";
    else {
        qDebug() << "MdGpsSerial: device failed to open";
        return false;
    }
    return true;
}

bool MdGpsSerial::changePortSettings (QString sport, QString speed) {
    if ( port )
        port->changePortSettings (sport, speed);
    closePort();
    return true;
}

void MdGpsSerial::incomingData( const QByteArray& bytes) {
    int a = bytes.size();
    //    qDebug() << "MdGpsSerial: bytes read:" << bytes.size();
    //    qDebug() << "MdGpsSerial: bytes:" << bytes;
    QString s (bytes);
    emit bytesRead ( bytes );

    QStringList sl = s.split (QRegExp("\\r\\n]") );
    QString rs;
    for ( int i = 1 ; i <= sl.size() ; i++ ) {
        bool t = true;
        if ( i == sl.size() ) {
            if ( bytes.size() >= 2 && !(bytes[a-2] == '\r') && !(bytes[a-1]=='\n') ) {
                buffer += sl[i-1];
                t = false;
            }
        }
        if ( t ) {
            if ( !buffer.isEmpty() ) {
                rs = buffer;
                buffer.clear();
            }
            rs += sl[i-1];
            parseLine (rs);
            //    		emit lineRead ( QString (rs) ) ;
        }
    }
}

void MdGpsSerial::parseLine ( const QString &l ) {
//    qDebug() << "MdGpsSerial::parseLine [" << l << "]";
    /*
"$GPGGA,232241.000,4909.4071,N,00702.2012,E,1,7,1.34,207.8,M,47.8,M,,*5D
$GPGSA,M,3,09,08,26,05,28,07,15,,,,,,1.61,1.34,0.90*0E
$GPGSV,4,1,13,09,66,057,33,08,64,056,30,26,64,300,23,05,58,215,16*72
$GPGSV,4,2,13,28,48,131,30,07,30,059,25,15,21,291,34,10,18,164,*75
$GPGSV,4,3,13,21,09,322,,19,06,046,,03,04,024,,27,03,015,*7A
$GPGSV,4,4,13,43,,,*7C
$GPRMC,232241.000,A,4909.4071,N,00702.2012,E,0.33,26.15,201013,,,A*59
"
*/

    bool allData = true;
#if defined ( Q_WS_MAEMO_5 )
    posInfo.setCoordinate(coordinate);
#endif
//    parseBuffer.append (l);
    parseBuffer = l;
    //XXYY.ZZZZ
    QRegExp WGS84 ("(\\d\\d)(\\d\\d\\.\\d\\d\\d\\d)");
    //$GPRMC,HHMMSS,A,BBBB.BBBB,b,LLLLL.LLLL,l,GG.G,RR.R,DDMMYY,M.M,m,F*PP
    //$GPRMC,213337.500,A,4909.4154,N,00702.2017,E,0.19,63.13,211013,,,A*52
    QRegExp gprmcR ("\\$GPRMC,([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w\\*]*)\\r\\n");
    int match = gprmcR.indexIn(parseBuffer);
    if ( match > -1 ) {
        QTime gpsTime = QTime::fromString(gprmcR.cap(1), "hhmmss.zzz");
        bool gpsValid = false;
        if (gprmcR.cap(2) == "A")
            gpsValid = true;
        QString latitude = gprmcR.cap(3);
        double latitudeDeg=666;
        if ( WGS84.indexIn(latitude) > -1 ) {
            //XXYY.ZZZZ ⇒ XX° + (YY.ZZZZ / 60)°
            latitudeDeg = WGS84.cap(1).toInt() + ( WGS84.cap(2).toDouble() / 60 );
        }
        //A positive latitude indicates the Northern Hemisphere, and a negative latitude indicates the Southern Hemisphere.
        QString latNS = gprmcR.cap(4);
        if ( latNS == "S")
            latitudeDeg = latitudeDeg * -1;

        QString longitude = gprmcR.cap(5);
        double longitudeDeg=666;
        if ( WGS84.indexIn(longitude) > -1 ) {
            //XXYY.ZZZZ ⇒ XX° + (YY.ZZZZ / 60)°
            longitudeDeg = WGS84.cap(1).toInt() + ( WGS84.cap(2).toDouble() / 60 );
        }
        //A positive longitude indicates the Eastern Hemisphere, and a negative longitude indicates the Western Hemisphere.
        QString longEW = gprmcR.cap(6);
        if ( longEW == "W")
            longitudeDeg = longitudeDeg * -1;

        double speedKnots = gprmcR.cap(7).toDouble();
        double speedKmh = speedKnots * 1.852;
        double courseDeg = gprmcR.cap(8).toDouble();
        QDate gpsDate = QDate::fromString(gprmcR.cap(9), "ddMMyy");
//        if ( gpsValid )
//            qDebug() << "GPRMC time=" << gpsTime.toString() << " lat=" << latitude << latNS << " long=" << longitude << longEW << " speed[km/h]=" << speedKmh;
//        else
//            qDebug() << "GPRMC invalid";

#if defined ( Q_WS_MAEMO_5 )
        coordinate = QGeoCoordinate(latitudeDeg, longitudeDeg);
        //docu says ground speed, in metres/sec.
        //but internal gps gives km/h
        //-> set km/h to be compatible
//        posInfo.setAttribute(QGeoPositionInfo::GroundSpeed, speedKnots * 0.514444 * 3.6);
        posInfo.setAttribute(QGeoPositionInfo::GroundSpeed, speedKmh);
        //The timestamp must be in UTC time.
        posInfo.setTimestamp( QDateTime(gpsDate, gpsTime, Qt::UTC) );
#endif
//        parseBuffer.remove(match, gprmcR.matchedLength());
    } else {
        allData = false;
    }
    //Global Positioning System Fix Data
    //$GPGGA,212634.000,4909.4139,N,00702.2037,E,1,9,1.40,202.7,M,47.8,M,,*54
    QRegExp gpggaR ("\\$GPGGA,([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w]*),([\\.\\w\\*]*)");
    match = gpggaR.indexIn(parseBuffer);
    if ( match > -1 ) {
        QTime gpsTime = QTime::fromString(gpggaR.cap(1), "hhmmss.zzz");
        double latitude = gpggaR.cap(2).toDouble();
        QString latNS = gpggaR.cap(3);
        double longitude = gpggaR.cap(4).toDouble();
        QString longEW = gpggaR.cap(5);
        // fix quality - 0 = Invalid / 1 = GPS fix / 2 = DGPS fix
        quint8 fixQuality = gpggaR.cap(6).toInt();
        quint8 numSatInView = gpggaR.cap(7).toInt();
        //Horizontal Dilution of Precision (HDOP)	1.5	Relative accuracy of horizontal position
        double hdop = gpggaR.cap(8).toDouble();
        //Altitude	280.2, M	280.2 meters above mean sea level
        double altitude = gpggaR.cap(9).toDouble();
        //Height of geoid above WGS84 ellipsoid	-34.0, M	-34.0 meters
        double heigthAboveWGS84 = gpggaR.cap(10).toDouble();
        //Time since last DGPS update	blank	No last update
        double timeSinceLastDGPSUpdate = gpggaR.cap(11).toDouble();
        //DGPS reference station id	blank	No station id
        double DGPSrefStationId = gpggaR.cap(12).toDouble();
        //Checksum	*75	Used by program to check for transmission errors
        QString checksum = gpggaR.cap(13);
#if defined ( Q_WS_MAEMO_5 )
        coordinate.setAltitude(altitude);
#endif
    } else {
        allData = false;
//        qDebug() << "GPGGA invalid";
    }
    //$GPGSV
    //GPS Satellites in view

#if defined ( Q_WS_MAEMO_5 )
    if ( allData ) {
        //we have valid Data!
//        qDebug() << "valid data " << posInfo.coordinate().toString();

        /* TODO
        QGeoPositionInfo::Direction
        QGeoPositionInfo::HorizontalAccuracy
        QGeoPositionInfo::VerticalAccuracy
        */

        emit positionUpdated(posInfo);
    }
#endif

}


void MdGpsSerial::closePort()
{
    if ( port )
        port->closePort();
    emit showStatusMessage ("MdGpsSerial: Serial Port closed");
    emit portClosed();
}


void MdGpsSerial::openPort()
{
    if ( port ) {
        port->openPort();
    }
//    qDebug("MdGpsSerial: is open: %d", port->isOpen());
//    if ( port->isOpen() ) {
//        emit showStatusMessage( "MdGpsSerial: Serial Port opened" );
//        emit portOpened();
//    } else {
//        emit showStatusMessage( "MdGpsSerial: Could not open Serial Port" );
//    }
}
