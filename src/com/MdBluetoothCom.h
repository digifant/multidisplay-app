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

#ifndef MDBLUETOOTHCOM_H
#define MDBLUETOOTHCOM_H

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothSocket>

#include <com/MdAbstractCom.h>

class QListWidgetItem;

class MdBluetoothCom : public MdAbstractCom
{
public:
    MdBluetoothCom( QObject* parent );
    ~MdBluetoothCom();

//signals:
//    void showStatusMessage ( const QString& );
//    void showStatusBarSampleCount ( const QString& );

//    void portOpened();
//    void portClosed();

//    //! new data received
//    void bytesRead ( const QByteArray & );

public slots:
    enum ServiceDiscoveryNeeded { No = 0, Uuid = 1, Name = 2, Yes = 4 };

    /**
     * @brief searches a spp profile with name "mdv2*" and connect to it (no service discovery!)
     */
    void sppConnect();

    /**
     * @brief sppConnect: connects to a the service with the given uuid
     * @param uuid
     */
    void sppConnect(const QString &uuid);
    void sppConnect(const QBluetoothServiceInfo &serviceInfo);

    void closePort();
    void openPort();

    /**
     * @brief setup and open the connection. a service discovery is triggered if the requested service isnt already discovered.
     * @param sport uuid of the bluetooth service. if empty a spp service with name starting with "mdv2*" is searched and connected
     * @param speed unused!
     * @return
     */
    bool setupPort (QString sport="mdv2turbo", QString speed="0");
    bool changePortSettings (QString sport="mdv2turbo", QString speed="0");

    void transmitMsg( const QString &s);
    void transmitMsg( const QByteArray &ba);


    void startServiceDiscovery(bool force=false);
    void serviceDiscoveryCanceled();
    void serviceDiscovered(const QBluetoothServiceInfo &serviceInfo);
    void serviceDiscoveryError(QBluetoothServiceDiscoveryAgent::Error);
    void serviceDiscoveryFinished();

protected slots:
    void onReadyRead();

    void connected();
    void disconnected();

protected:

private:
    QBluetoothServiceDiscoveryAgent *sDiscoveryAgent;
    QBluetoothSocket *socket;
    QString localDeviceName;
    //connected devices
    QList<QBluetoothAddress> remotes;
    QMap<QListWidgetItem*, QBluetoothServiceInfo> m_discoveredServices;
    ServiceDiscoveryNeeded sdNeeded;
    QString uuid;
    QString mdServiceName;
};

#endif // MDBLUETOOTHCOM_H
