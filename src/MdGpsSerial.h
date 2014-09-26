#ifndef MDGPSSERIAL_H
#define MDGPSSERIAL_H


#include <QObject>

#if defined ( Q_WS_MAEMO_5 )
#include <QGeoPositionInfo>
QTM_USE_NAMESPACE
#endif

class QextSerialPort;
class QTimer;



/**
  MdGpsSerial receives and parses a serial NMEA data stream
  */
class MdGpsSerial : public QObject {
    Q_OBJECT
public:
    MdGpsSerial();
    virtual ~MdGpsSerial();

    bool changePortSettings (QString sport="/dev/rfcomm5", QString speed="115200");

public slots:
    void closePort();
    void openPort();

protected slots:
    virtual void onReadyRead();
    void onDsrChanged(bool status);

signals:
    void bytesRead (QByteArray);
    void stringRead (QString);
    void lineRead (QString);
    void showStatusMessage (QString);

    void portOpened();
    void portClosed();

#if defined ( Q_WS_MAEMO_5 )
    void positionUpdated(const QGeoPositionInfo &update);
#endif

protected:

    bool setupPort (QString sport="/dev/rfcomm5", QString speed="115200");

    QextSerialPort *port;
    QString buffer;
    QString parseBuffer;

    void parseLine (QString l);

#if defined ( Q_WS_MAEMO_5 )
    QGeoCoordinate coordinate;
    QGeoPositionInfo posInfo;
#endif
};


#endif // MDGPSSERIAL_H
