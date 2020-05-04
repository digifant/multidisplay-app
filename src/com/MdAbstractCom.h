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

#ifndef MDABSTRACTCOM_H
#define MDABSTRACTCOM_H

#include <QObject>

class MdAbstractCom : public QObject
{
    Q_OBJECT
public:
    explicit MdAbstractCom(QObject *parent = nullptr);
    virtual ~MdAbstractCom ();

    void setAutoReconnect ( bool r) { autoReconnect = r; }
    bool getAutoReconnect ()  { return autoReconnect; }

signals:
    void showStatusMessage ( const QString& );
    void showStatusBarSampleCount ( const QString& );

    void portOpened();
    void portClosed();

    //! BT wrapper class needs this to get info that BT classic or LE can not find any device
    void couldNotConnect2BtDevice();
    void bluetoothNotAvailable();

    //! new data received
    void bytesRead ( const QByteArray & );


public slots:
    virtual void togglePort() = 0;
    virtual void closePort() = 0;
    virtual void openPort() = 0;

    virtual bool setupPort (QString sport="/dev/ttyUSB0", QString speed="115200") = 0;
    virtual bool changePortSettings (QString sport="/dev/ttyUSB0", QString speed="115200") = 0;

    virtual void transmitMsg( const QString &s) = 0;
    virtual void transmitMsg( const QByteArray &ba) = 0;

protected slots:
    virtual void onReadyRead() = 0;

protected:
    bool autoReconnect = true;

};

#endif // MDABSTRACTCOM_H
