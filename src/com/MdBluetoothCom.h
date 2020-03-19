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
    Q_OBJECT
public:
    /**
     * @brief MdBluetoothCom starts service discovery and connects to spp service if service is found
     * @param parent
     */
    MdBluetoothCom( QObject* parent = nullptr, const QString n="mdv2" );
    virtual ~MdBluetoothCom();

    enum bluetoothState {
           Idle = 0,
           Scanning,
           ScanFinished,
           Connecting,
           Connected,
           ServiceFound,
           AcquireData
    };
    Q_ENUM(bluetoothState)
    enum ServiceDiscoveryNeeded { No = 0, Uuid = 1, Name = 2, Yes = 4 };

    void setState(MdBluetoothCom::bluetoothState newState);
    virtual void connectForWrapper();
    void stopSearchForWrapper();

signals:
    /* Signals for user */
    // FIXME needed??
    void changedState(MdBluetoothCom::bluetoothState newState);
    // FIXME needed??
//    void showStatusMessage ( const QString& );
//    void showStatusBarSampleCount ( const QString& );

//    void portOpened();
//    void portClosed();

//    //! new data received
//    void bytesRead ( const QByteArray & );


public slots:

    /**
     * @brief searches a spp profile with name "mdv2*" and connect to it (no service discovery!)
     */
    bool sppConnect();

    /**
     * @brief sppConnect: connects to a the service with the given uuid
     * @param uuid
     */
    bool sppConnect(const QString &uuid);
    bool sppConnect(const QBluetoothServiceInfo &serviceInfo);

    void togglePort();

    void closePort();
    void openPort();

    /**
     * @brief setup and open the connection. a service discovery is triggered if the requested service isnt already discovered.
     * @param sport uuid of the bluetooth service. if empty a spp service with name starting with "mdv2*" is searched and connected
     * @param speed unused!
     * @return
     */
    bool setupPort (QString sport="", QString speed="0");
    bool changePortSettings (QString sport="", QString speed="0");

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
    void socketStateChanged (QBluetoothSocket::SocketState);

    void emitBtNotAvailableDeferred();

protected:

private:
    QBluetoothServiceDiscoveryAgent *sDiscoveryAgent = nullptr;
    QBluetoothSocket *socket = nullptr;
    QString localDeviceName;
    //connected devices
    QList<QBluetoothAddress> remotes;
    QMap<QListWidgetItem*, QBluetoothServiceInfo> m_discoveredServices;
    ServiceDiscoveryNeeded sdNeeded = Yes;
    QString uuid;
    //dont needed: remove or make a list
    QString deviceName = QString("mdv2");
    MdBluetoothCom::bluetoothState m_state = bluetoothState::Idle;
};

#endif // MDBLUETOOTHCOM_H
