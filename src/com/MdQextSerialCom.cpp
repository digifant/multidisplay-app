/*
    Copyright 2014 Stephan Martin, Dominik Gummel

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

#include <QDebug>
#include "MdQextSerialCom.h"

#include <qextserialport.h>


MdQextSerialCom::MdQextSerialCom(QObject *parent) : MdAbstractCom(parent), port(0)
{
}

MdQextSerialCom::~MdQextSerialCom()
{
    if ( port ) {
        delete port;
        port = 0;
    }
}


bool MdQextSerialCom::setupPort (QString sport, QString speed) {
    if ( port )
        delete port;
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
//        connect(port, SIGNAL(dsrChanged(bool)), this, SLOT(onDsrChanged(bool)));
        qDebug() << "listening for data on" << port->portName();
        emit portOpened();
    } else {
        qDebug() << "device failed to open:" << port->errorString();
        return false;
    }
    return true;
}

bool MdQextSerialCom::changePortSettings (QString sport, QString speed) {
    closePort();
    setupPort (sport, speed);
    qDebug() << "changed serial port to " << sport;
    return true;
}


void MdQextSerialCom::transmitMsg(const QString &s) {
    port->write(s.toAscii(), s.length());
}

void MdQextSerialCom::transmitMsg(const QByteArray &ba)
{
    port->write(ba);
}

void MdQextSerialCom::onReadyRead()
{
    QByteArray bytes;
    int bc = port->bytesAvailable();
    bytes.resize(bc);
    port->read(bytes.data(), bytes.size());
    emit bytesRead( bytes );
}




void MdQextSerialCom::closePort()
{
    if ( port ) {
        port->close();
        qDebug("is open: %d", port->isOpen());
    }
    emit showStatusMessage ("Serial Port closed");
    emit portClosed();
}


void MdQextSerialCom::openPort()
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
