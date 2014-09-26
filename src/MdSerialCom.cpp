/*
    Copyright 2010 Stephan Martin, Dominik Gummel

    This file is part of Multidisplay.

    Multidisplay is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Multidisplay is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Multidisplay.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "MdSerialCom.h"

#include "MdData.h"
#include "MdDataRecordV2.h"

#include "qextserialport.h"
#include "qdebug.h"
#include <QRegExp>
#include <QStringList>
#include <QLabel>
#include <QTimer>

#define _TTY_POSIX_
//#define DEBUG 1

SerialData::SerialData () : fixedfac(1000) {
}

int SerialData::getFixed32 (int n) {
    return floats[n] * fixedfac;
}



MdSerialCom::MdSerialCom(MdData* data) : data(data) {
    setupPort();

    ap.append(2);
    ap.append(1);
    ah.append(2);
    ah.append(2);
    bp.append(2);
    bp.append(3);
    //    bh={2,4};
    bh.append(2);
    bh.append(3);

    mdso.append(3);
    mdso.append(2);

    mdcalboost.append(4);
    mdcalboost.append(2);
    mdsave.append(4);
    mdsave.append(1);
    mdload.append(4);
    mdload.append(3);

    cbr = NULL;

#ifdef DEBUG
    debugDataGenTimer = new QTimer(this);
    connect ( debugDataGenTimer, SIGNAL(timeout()), this, SLOT(debugDataGenUpdate()) );
    debugDataGenTimer->start (1000);
    debugRPMCounter=0;
    debugTime = 0;
#endif
}

MdSerialCom::~MdSerialCom() {
    delete (port);
    port = NULL;
}

bool MdSerialCom::setupPort (QString sport, QString speed) {
    //modify the port settings on your own
    port = new QextSerialPort(sport, QextSerialPort::EventDriven);
    if ( speed == "115200")
        port->setBaudRate(BAUD115200);
    else
        port->setBaudRate(BAUD57600);
    port->setFlowControl(FLOW_OFF);
    port->setParity(PAR_NONE);
    port->setDataBits(DATA_8);
    port->setStopBits(STOP_1);
    //set timeouts to 500 ms
    port->setTimeout(500);

    if (port->open(QIODevice::ReadWrite) == true) {
        connect(port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(port, SIGNAL(dsrChanged(bool)), this, SLOT(onDsrChanged(bool)));
        if (!(port->lineStatus() & LS_DSR)) {
            qDebug() << "warning: device is not turned on";
            return false;
        }
        qDebug() << "listening for data on" << port->portName();
        emit portOpened();
    }
    else {
        qDebug() << "device failed to open:" << port->errorString();
        return false;
    }
    return true;
}

bool MdSerialCom::changePortSettings (QString sport, QString speed) {
    closePort();
    delete (port);
    setupPort (sport, speed);
    qDebug() << "changed serial port to " << sport;
    return true;
}

void MdSerialCom::debugDataGenUpdate() {
    debugTime += 1000;
    debugRPMCounter += 100;
    if ( debugRPMCounter > 7500 )
        debugRPMCounter = 850;
    int thr = 0;
    if ( debugRPMCounter > 3000 )
        thr = 1;
    MdSensorRecord *csr = new MdSensorRecord ();
    csr->setTime(debugTime);
    csr->setRpm(debugRPMCounter);
    csr->setThrottle(thr);
    csr->setBoost (debugRPMCounter / 3250 );

    data->addDataRecord( new MdDataRecord (csr) );
    emit showStatusBarSampleCount ( QString::number (data->size()) );
}

void MdSerialCom::onReadyRead()
{
    QByteArray bytes;
    int a = port->bytesAvailable();
    bytes.resize(a);
    port->read(bytes.data(), bytes.size());
    //    qDebug() << "bytes read:" << bytes.size();
    //    qDebug() << "bytes:" << bytes;
    QString s (bytes);
    emit bytesRead ( bytes );

    QStringList sl = s.split (QRegExp("\\r\\n]") );
    QString rs;
    for ( int i = 1 ; i <= sl.size() ; i++ ) {
        bool t = true;
        if ( i == sl.size() ) {
            if ( bytes.size() >= 2 && !(bytes[a-2] == '\r') && !(bytes[a-1]=='\n') ) {
                ts += sl[i-1];
                t = false;
            }
        }
        if ( t ) {
            if ( !ts.isEmpty() ) {
                rs = ts;
                ts.clear();
            }
            rs += sl[i-1];
            //    		rs.remove ( QRegExp("[\\r\\n]") );
            parseLine (rs);
            //			qDebug() << "parseLine ( " << rs << " )";

            //    		rs.remove ( QRegExp("\\n") );
            //    		rs.remove ( '\2' );
            //    		rs.remove ( '\3' );
            //    		emit lineRead ( QString (rs) ) ;
        }
    }

}

void MdSerialCom::parseLine (QString l) {

    qDebug() << "MdSerialCom is DEPRECATED! Dont use it!";
//    buffer.append (l);
//    QRegExp dataR ("\\02([\\d\\s\\w\\.\\-;:]+)\\03");
//    //PID 1.00 -1.03 255.00 2.00 5.00 1.00 Automatic
//    QRegExp pidR ("PID (\\-*\\d*\\.\\d+) (\\-*\\d*\\.\\d+) (\\-*\\d*\\.\\d+) (\\-*\\d*\\.\\d+) (\\-*\\d*\\.\\d+) (\\-*\\d*\\.\\d+) (\\w+)");
//    //2:Time;RPM;Boost;Throttle;Lambda;LMM;CaseTemp;TypK1;TypK2;Battery;VDOPres1;VDOPres2;VDOPres3;VDOTemp1;VDOTemp2;VDOTemp3
//    //2:22276;0;-1.03;0;0.68;0.00;0.00;0;0;5.12;int;int;int;int;int;int
//    QRegExp convertedR ("2:([\\-\\d\\.]*);([\\-\\d\\.]*);([\\-\\d\\.]*);([\\-\\d\\.]*);([\\-\\d\\.]*);([\\-\\d\\.]*);([\\-\\d\\.]*);([\\-\\d\\.]*);([\\-\\d\\.]*);([\\-\\d\\.]*);([\\-\\d\\.]*);([\\-\\d\\.]*);([\\-\\d\\.]*);([\\-\\d\\.]*);([\\-\\d\\.]*);([\\-\\d\\.]*)");
//    for ( int i = 0 ; i != -1 ; i = dataR.indexIn(buffer, i) ) {
//        if ( dataR.cap(1).length() > 0 ) {
//            QString ds = dataR.cap(1);
//            buffer.remove(0,i-1 + dataR.matchedLength());
//            emit lineRead ( ds ) ;

//            if ( pidR.indexIn(ds,0) > -1 ) {
//                //				qDebug ("pid matched");
//                cbr = new MdBoostPidRecord( QString( pidR.cap(1)).toDouble(),
//                                            QString( pidR.cap(2)).toDouble(),
//                                            QString( pidR.cap(3)).toDouble(),
//                                            QString( pidR.cap(4)).toDouble(),
//                                            QString( pidR.cap(5)).toDouble(),
//                                            QString( pidR.cap(6)).toDouble(),
//                                            QString( pidR.cap(7)) );
//                //				ui->BoostPIDTableWidget->item(0,0)->setText( QString::number(cbr->getSetPoint()) );
//                //				ui->BoostPIDTableWidget->item(0,1)->setText( QString::number(cbr->getInput()) );
//                //				ui->BoostPIDTableWidget->item(0,2)->setText( QString::number(cbr->getOutput()) );
//                //				ui->BoostPIDTableWidget->item(0,3)->setText( QString::number(cbr->getPParam()) );
//                //				ui->BoostPIDTableWidget->item(0,4)->setText( QString::number(cbr->getIParam()) );
//                //				ui->BoostPIDTableWidget->item(0,5)->setText( QString::number(cbr->getDParam()) );
//                //				ui->BoostPIDTableWidget->item(0,6)->setText( cbr->getMode() );
//                emit newBoostPidMsg(cbr);
//            } else if ( convertedR.indexIn(ds,0) > -1 ) {
//                //				qDebug ("data matched");
//                //				qDebug ( QString( convertedR.cap(0) ).toAscii() );
//                //				qDebug ( QString( convertedR.cap(1) ).toAscii() );
//                csr = new compatibility::MdSensorRecordV2 ( QString( convertedR.cap(1)).toInt(), QString( convertedR.cap(2)).toInt(),
//                                           QString( convertedR.cap(4)).toInt(), QString( convertedR.cap(3)).toDouble(),
//                                           QString( convertedR.cap(5)).toDouble(), QString( convertedR.cap(6)).toDouble(),
//                                           QString( convertedR.cap(7)).toDouble(), QString( convertedR.cap(8)).toDouble(),
//                                           QString( convertedR.cap(9)).toDouble(), QString( convertedR.cap(10)).toDouble(),
//                                           QString( convertedR.cap(11)).toDouble() / 10.0, QString( convertedR.cap(12)).toDouble() / 10.0,
//                                           QString( convertedR.cap(13)).toDouble() / 10.0, QString( convertedR.cap(14)).toDouble() / 10.0,
//                                           QString( convertedR.cap(15)).toDouble() / 10.0, QString( convertedR.cap(16)).toDouble() / 10.0 );
//                if ( cbr == NULL )
//                    cbr = new MdBoostPidRecord();
//                data->addDataRecord( new MdDataRecord (cbr, csr) );
//#ifndef Q_WS_MAEMO_5
//                emit showStatusMessage( QString::number (data->size()) );
//#endif
//                cbr = NULL;
//            }
//        }
//    }
}

void MdSerialCom::onDsrChanged(bool status)
{
    if (status)
        qDebug() << "device was turned on";
    else
        qDebug() << "device was turned off";
}


void MdSerialCom::transmitMsg(QString s) {
    port->write(s.toAscii(), s.length());
    //	int i = port->write(s.toAscii(), s.length());
    //	qDebug("transmitted : %d", i);
}

void MdSerialCom::transmitMsg(QByteArray ba)
{
    port->write(ba);
    //	int i = port->write(ba);
    //	qDebug("transmitted : %d", i);
}

void MdSerialCom::mdCmdAp() {
    transmitMsg (ap);
}
void MdSerialCom::mdCmdAh() {
    transmitMsg (ah);
}
void MdSerialCom::mdCmdBp() {
    transmitMsg (bp);
}
void MdSerialCom::mdCmdBh() {
    transmitMsg (bh);
}

void MdSerialCom::mdCmdActivateSerialOutput() {
    transmitMsg (mdso);
}
void MdSerialCom::mdCmdActivateSerialBinaryOutput() {
    QByteArray t;
    t.push_back (3);
    t.push_back (4);
    qDebug() << "cmd transmit length=" << t.length();
    transmitMsg(t);
}

void MdSerialCom::mdCmdDisableSerialOutput() {
    QByteArray t;
    t.push_back (3);
    t.push_back ( (char) 0);
    qDebug() << "cmd transmit length=" << t.length();
    transmitMsg(t);
}

void MdSerialCom::mdCmdCalBoost() {
    transmitMsg (mdcalboost);
}
void MdSerialCom::mdCmdLoadFromEeprom() {
    transmitMsg (mdload);
}
void MdSerialCom::mdCmdSave2Eeprom() {
    transmitMsg (mdsave);
}
void MdSerialCom::mdCmdReadEeprom() {
    transmitMsg (mdload);
}
void MdSerialCom::mdCmdN75ManualDuty (int normal, int race) {
    QByteArray t;
    t.push_back (4);
    t.push_back (4);
    t.push_back ( (quint8) normal );
    t.push_back ( (quint8) race );
    qDebug() << "cmd transmit length=" << t.length();
    transmitMsg(t);
}

void MdSerialCom::mdCmdBoostPidParam (double setpoint, int output, double p, double i, double d, bool autoMode) {
    QByteArray t;

    if (autoMode)
        t.push_back (1);
    else
        t.push_back ( (char) 0);

    SerialData sd;
    sd.floats[0] = (float) setpoint;
    //1 input (discarded)
    sd.floats[1] = (float) 0.0;
    sd.floats[2] = (float) output;
    sd.floats[3] = (float) p;
    sd.floats[4] = (float) i;
    sd.floats[5] = (float) d;


    for ( int v = 0 ; v < 6 ; v++ ) {
        union {
            int i32;
            uchar asBytes[4];
        } f;
        f.i32 = sd.getFixed32(v);
        for ( int w = 0 ; w < 4 ; w++ )
            t.push_back (f.asBytes[w]);
    }


    qDebug() << "cmd transmit length=" << t.length();
    transmitMsg(t);
    //	port->write(msg, 25);
    //	free (msg);
}

void MdSerialCom::receiveMsg()
{
    char buff[1024];
    int numBytes;

    numBytes = port->bytesAvailable();
    if(numBytes > 1024)
        numBytes = 1024;

    int i = port->read(buff, numBytes);
    if (i != -1)
        buff[i] = '\0';
    else
        buff[0] = '\0';
    QString msg = buff;

    //	received_msg->append(msg);
    //	received_msg->ensureCursorVisible();
    qDebug("bytes available: %d", numBytes);
    qDebug("received: %d", i);
}

void MdSerialCom::closePort()
{
    port->close();
    qDebug("is open: %d", port->isOpen());
    emit showStatusMessage ("Serial Port closed");
    emit portClosed();
}


void MdSerialCom::openPort()
{
    port->open(QIODevice::ReadWrite);
    qDebug("is open: %d", port->isOpen());
    if ( port->isOpen() ) {
        emit showStatusMessage( "Serial Port opened" );
        emit portOpened();
    } else {
        emit showStatusMessage( "Could not open Serial Port" );
    }
}
