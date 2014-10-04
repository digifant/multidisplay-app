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

#ifndef MDQSERIALPORTCOM_H
#define MDQSERIALPORTCOM_H

#include "com/MdAbstractCom.h"

class QSerialPort;

class MdQSerialPortCom : public MdAbstractCom
{
public:
    MdQSerialPortCom(QObject *parent = 0);
    ~MdQSerialPortCom();

public slots:
    void togglePort();
    void closePort();
    void openPort();

    bool setupPort (QString sport="/dev/ttyUSB0", QString speed="115200");
    bool changePortSettings (QString sport="/dev/ttyUSB0", QString speed="115200");

    void transmitMsg( const QString &s );
    void transmitMsg( const QByteArray &ba );


protected slots:
    void onReadyRead();

protected:
    QSerialPort *port;
};

#endif // MDQSERIALPORTCOM_H
