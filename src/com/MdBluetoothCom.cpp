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

#include "MdBluetoothCom.h"

#include <QListWidgetItem>
#include <QBluetoothLocalDevice>
#include <QTimer>

MdBluetoothCom::MdBluetoothCom(QObject *parent, const QString n)
    : MdAbstractCom(parent), deviceName(n)
{

    QBluetoothLocalDevice localDevice;

    // Check if Bluetooth is available on this device
    if (localDevice.isValid()) {

        // Turn Bluetooth on
        localDevice.powerOn();

        // Read local device name
        localDeviceName = localDevice.name();
        qDebug() << "local bluetooth device name: " << localDeviceName;
        // Make it visible to others
        //localDevice.setHostMode(QBluetoothLocalDevice::HostDiscoverable);

        // Get connected devices
        remotes = localDevice.connectedDevices();

        startServiceDiscovery();

    } else {
        qDebug() << "bluetooth not available!";
        QTimer::singleShot(500, this, SLOT(emitBtNotAvailableDeferred()));
    }
}

MdBluetoothCom::~MdBluetoothCom() {
    if ( socket != nullptr ) delete socket;
    if ( sDiscoveryAgent != nullptr ) delete sDiscoveryAgent;
    //qDeleteAll(m_discoveredServices);
}

void MdBluetoothCom::connectForWrapper(){
    setState(Idle);
    startServiceDiscovery();
}
void MdBluetoothCom::stopSearchForWrapper() {
    if ( sDiscoveryAgent != nullptr )
        sDiscoveryAgent->stop();
}

void MdBluetoothCom::setState(MdBluetoothCom::bluetoothState newState)
{
    if (m_state == newState)
        return;

    qDebug() << "new state " << newState;
    m_state = newState;
    emit changedState(newState);
}

bool MdBluetoothCom::sppConnect() {
    foreach ( QBluetoothServiceInfo si, m_discoveredServices.values() ) {
        if ( ( ( si.device().name().startsWith(deviceName,Qt::CaseInsensitive) ) || ( si.device().name().startsWith("df1ecu",Qt::CaseInsensitive) ) )
             && ( si.serviceName() == "Serial Port Profile") ) {
            //FIX one HC has Service name: "Dev B"
            // we found our md bluetooth service!
            qDebug() << "found mdv2 service on " << si.device().name() << " " << si.device().address().toString();
            qDebug() << "Service name:" << si.serviceName() << " UUID=" << si.serviceUuid().toString();
            if ( sppConnect(si) )
                return true;
        }
    }
    return false;
}

bool MdBluetoothCom::sppConnect(const QString &uuid) {
    Q_UNUSED(uuid)
    QBluetoothServiceInfo s;
    s.setServiceUuid( QBluetoothUuid(QString("{00001101-0000-1000-8000-00805f9b34fb}")) );
    return sppConnect(s);
}

bool MdBluetoothCom::sppConnect(const QBluetoothServiceInfo &serviceInfo) {
    setState(bluetoothState::Connecting);
    if (!socket) {
        socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
        connect (socket, SIGNAL(stateChanged(QBluetoothSocket::SocketState)), this, SLOT(socketStateChanged(QBluetoothSocket::SocketState)) );
        qDebug() << "Created socket";
    }

    socket->connectToService(serviceInfo);
    setState(bluetoothState::Connected);
    qDebug() << "ConnectToService done";

    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );

    return true;
}

void MdBluetoothCom::togglePort()
{
    if ( socket ) {
//        if ( socket->isOpen() )
//            closePort();
//        else {
//            openPort();
//        }
        switch ( socket->state() ) {
        case QBluetoothSocket::ConnectedState:
            closePort();
            break;
        case QBluetoothSocket::UnconnectedState:
            setupPort();
            break;
        }
    } else {
        setupPort();
    }
}

void MdBluetoothCom::closePort()
{
    if ( socket ) {
        socket->disconnectFromService();
//        socket->close();
        qDebug("is open: %d", socket->isOpen());
    }
    setState(MdBluetoothCom::bluetoothState::Idle);
    emit showStatusMessage ("Bluetooth: SPP closed");
    emit portClosed();
}


void MdBluetoothCom::openPort()
{

    socket->open(QIODevice::ReadWrite);
    qDebug("is open: %d", socket->isOpen());
    if ( socket->isOpen() ) {
        if ( socket->state()==QBluetoothSocket::ConnectedState ) {
            emit showStatusMessage( "Bluetooth: SPP opened" );
            emit portOpened();
        }
    } else {
        emit showStatusMessage( "Bluetooth: Could not open spp socket" );
    }
}

bool MdBluetoothCom::setupPort(QString sport, QString speed) {
    Q_UNUSED(speed)
    sdNeeded = No;
    if ( sport == "" ) {
        sdNeeded = Name;
        //mdv2
        /*
         * D/digifantview(19481): Discovered service on  "df1ecu"   "98:D3:31:30:27:22"
           D/digifantview(19481): 	Service name: "Serial Port Profile"
           D/digifantview(19481): 	Description: ""
           D/digifantview(19481): 	Provider: ""
           D/digifantview(19481): 	L2CAP protocol service multiplexer: 0
           D/digifantview(19481): 	RFCOMM server channel: 0
           D/digifantview(19481): Discovered service on  "df1ecu"   "98:D3:31:30:27:22"
           D/digifantview(19481): 	Service name: ""
           D/digifantview(19481): 	Description: ""
           D/digifantview(19481): 	Provider: ""
           D/digifantview(19481): 	L2CAP protocol service multiplexer: 0
           D/digifantview(19481): 	RFCOMM server channel: -1
           D/digifantview(19481): service discovery finished
           D/digifantview(19481):
           D/digifantview(19481): found mdv2 service on  "df1ecu"   "98:D3:31:30:27:22"
           D/digifantview(19481): Service name: "Serial Port Profile"  UUID= "{00001101-0000-1000-8000-00805f9b34fb}"
        */
        foreach ( QBluetoothServiceInfo si, m_discoveredServices.values() ) {
            if ( ( ( si.device().name().startsWith(deviceName,Qt::CaseInsensitive) ) || ( si.device().name().startsWith("df1ecu",Qt::CaseInsensitive) ) )
                 && ( si.serviceName() == "Serial Port Profile") ) {
                // we found our md bluetooth service!
                qDebug() << "found mdv2 service on " << si.device().name() << " " << si.device().address().toString();
                qDebug() << "Service name:" << si.serviceName() << " UUID=" << si.serviceUuid().toString();
                emit showStatusMessage( "Bluetooth: found mdv2 service on " + si.device().name() + " " + si.device().address().toString() +
                                        "Service name:" + si.serviceName() + " UUID=" + si.serviceUuid().toString() );
                setState(MdBluetoothCom::bluetoothState::Connecting);
                sppConnect(si);
                openPort();
                sdNeeded = No;
                return true;
            }
        }
        sdNeeded = Name;
    } else {
        //UUID
        sdNeeded = No;
        uuid = sport;
        sppConnect(sport);
        openPort();
    }
    startServiceDiscovery();
    return false;
}

bool MdBluetoothCom::changePortSettings(QString sport, QString speed)
{
    qDebug() << "MdBluetoothCom::changePortSettings " << sport << " " << speed;
    closePort();
    return setupPort(sport, speed);
}

void MdBluetoothCom::transmitMsg(const QString &s) {
    if ( socket )
        socket->write(s.toLatin1(), s.length());
}

void MdBluetoothCom::transmitMsg(const QByteArray &ba)
{
    if ( socket )
        socket->write(ba);
}

void MdBluetoothCom::startServiceDiscovery(bool force)
{
//    te->insertHtml("<span style=\"color:black; font-weight:normal\">start service discovery</span><br>");
    if (!sDiscoveryAgent) {
        sDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(this);

        connect( sDiscoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo) ),
            this, SLOT(serviceDiscovered(QBluetoothServiceInfo)) );
        connect(sDiscoveryAgent, SIGNAL(finished()), this, SLOT(serviceDiscoveryFinished()) );
        connect(sDiscoveryAgent, SIGNAL(canceled()), this, SLOT(serviceDiscoveryCanceled()) );
        connect(sDiscoveryAgent, SIGNAL(error(QBluetoothServiceDiscoveryAgent::Error)),
                this, SLOT(serviceDiscoveryError(QBluetoothServiceDiscoveryAgent::Error)) );
    }

    if ( (force) || !( sdNeeded==No ) ) {
        qDebug() << "start service discovery";
        sDiscoveryAgent->start();
        m_discoveredServices.clear();
        setState(MdBluetoothCom::bluetoothState::Scanning);
    }
}

void MdBluetoothCom::serviceDiscoveryCanceled()
{
    qDebug() << "service discovery canceld! ";
    emit couldNotConnect2BtDevice();
}

void MdBluetoothCom::serviceDiscovered(const QBluetoothServiceInfo &serviceInfo)
{
#if 1
    qDebug() << "Discovered service on "
             << serviceInfo.device().name() << " " << serviceInfo.device().address().toString();
    qDebug() << "\tService name:" << serviceInfo.serviceName();
    qDebug() << "\tDescription:"
             << serviceInfo.attribute(QBluetoothServiceInfo::ServiceDescription).toString();
    qDebug() << "\tProvider:"
             << serviceInfo.attribute(QBluetoothServiceInfo::ServiceProvider).toString();
    qDebug() << "\tL2CAP protocol service multiplexer:"
             << serviceInfo.protocolServiceMultiplexer();
    qDebug() << "\tRFCOMM server channel:" << serviceInfo.serverChannel();
#endif

    QMapIterator<QListWidgetItem *, QBluetoothServiceInfo> i(m_discoveredServices);
    while (i.hasNext()){
        i.next();
        if (serviceInfo.device().address() == i.value().device().address()){
            return;
        }
    }

    QString remoteName;
    if (serviceInfo.device().name().isEmpty())
        remoteName = serviceInfo.device().address().toString();
    else
        remoteName = serviceInfo.device().name();

    QListWidgetItem *item =
        new QListWidgetItem( QString::fromLatin1("%1 %2").arg(remoteName,serviceInfo.serviceName()) );
//    if ( btListWidget )
//        btListWidget->addItem(item);

    m_discoveredServices.insert(item, serviceInfo);
}

void MdBluetoothCom::serviceDiscoveryError(QBluetoothServiceDiscoveryAgent::Error error)
{
    qDebug() << "service discovery error! \n" << error;
}

void MdBluetoothCom::serviceDiscoveryFinished()
{
    qDebug() << "service discovery finished\n";
    switch (sdNeeded) {
        case Uuid:
            if ( ! sppConnect(uuid) )
                emit couldNotConnect2BtDevice();
            break;
        case Name:
        case Yes:
            if ( ! sppConnect() )
                emit couldNotConnect2BtDevice();
            break;
        default:
            emit couldNotConnect2BtDevice();
            break;
    }
}

void MdBluetoothCom::onReadyRead()
{
    QByteArray bytes;
    qint64 bc = socket->bytesAvailable();
    bytes.resize(bc);
    socket->read(bytes.data(), bytes.size());
    emit bytesRead( bytes );
}

void MdBluetoothCom::connected()
{
    emit portOpened();
    emit showStatusMessage ("Bluetooth connected!");
    setState(bluetoothState::AcquireData);
    qDebug() << "MdBluetoothCom::connected()";
}

void MdBluetoothCom::disconnected()
{
    emit portClosed();
    emit showStatusMessage ("Bluetooth disconnected!");
    setState(bluetoothState::Idle);
    qDebug() << "MdBluetoothCom::disconnected()";

    if ( getAutoReconnect() ) {
        //TODO auto reconnect ?
        //TODO TESTME!
        setupPort();
    }
}

void MdBluetoothCom::socketStateChanged(QBluetoothSocket::SocketState s)
{
    switch (s) {
    case QBluetoothSocket::ConnectedState:
        setState(bluetoothState::Connected);
        emit showStatusMessage( "Bluetooth: socket connected!");
        openPort();
        qDebug() << "socket connected";
        break;
    case QBluetoothSocket::UnconnectedState:
        setState(bluetoothState::Idle);
        emit showStatusMessage( "Bluetooth: socket unconnected!");
        closePort();
        qDebug() << "socket unconnected";
        break;
    case QBluetoothSocket::ConnectingState:
        setState(bluetoothState::Connecting);
        emit showStatusMessage( "Bluetooth: socket is attempting to connect to a device.!");
        qDebug() << "Socket is attempting to connect to a device";
        break;
    }
}

void MdBluetoothCom::emitBtNotAvailableDeferred()
{
    emit showStatusMessage( "Bluetooth: not available!" );
    emit bluetoothNotAvailable();
}
