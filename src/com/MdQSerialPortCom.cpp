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
#include "MdQSerialPortCom.h"

#include <QtSerialPort/QSerialPort>



MdQSerialPortCom::MdQSerialPortCom(QObject *parent) : MdAbstractCom(parent), port(0)
{
}

MdQSerialPortCom::~MdQSerialPortCom()
{
    if ( port ) {
        delete port;
        port = 0;
    }
}

void MdQSerialPortCom::togglePort()
{
    if ( port ) {
        if ( port->isOpen() )
            closePort();
        else {
            openPort();
        }
    } else {
        setupPort();
    }
}


bool MdQSerialPortCom::setupPort (QString sport, QString speed) {
    if ( port )
        delete port;
    port = new QSerialPort(sport, this);
    if ( speed == "115200")
        port->setBaudRate(QSerialPort::Baud115200);
    else
        port->setBaudRate(QSerialPort::Baud57600);
    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setParity(QSerialPort::NoParity);
    port->setDataBits(QSerialPort::Data8);
    port->setStopBits(QSerialPort::OneStop);

    if (port->open(QIODevice::ReadWrite) == true) {
        connect(port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        qDebug() << "listening for data on" << port->portName();
        emit portOpened();
    }
    else {
        qDebug() << "device failed to open:" << port->errorString();
        return false;
    }
    return true;
}


bool MdQSerialPortCom::changePortSettings (QString sport, QString speed) {
    closePort();
    setupPort (sport, speed);
    qDebug() << "changed serial port to " << sport;
    return true;
}


void MdQSerialPortCom::transmitMsg(const QString &s) {
    port->write(s.toLatin1(), s.length());
}

void MdQSerialPortCom::transmitMsg(const QByteArray &ba)
{
    port->write(ba);
}

void MdQSerialPortCom::onReadyRead()
{
    QByteArray bytes;
    int bc = port->bytesAvailable();
    bytes.resize(bc);
    port->read(bytes.data(), bytes.size());
    emit bytesRead( bytes );
}




void MdQSerialPortCom::closePort()
{
    if ( port ) {
        port->close();
        qDebug("is open: %d", port->isOpen());
    }
    emit showStatusMessage ("Serial Port closed");
    emit portClosed();
}


void MdQSerialPortCom::openPort()
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
