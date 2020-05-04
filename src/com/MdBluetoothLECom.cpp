#include "MdBluetoothLECom.h"

#include <QDebug>
#include <QMetaEnum>
#include <QTimer>
#include <QBluetoothLocalDevice>

MdBluetoothLECom::MdBluetoothLECom(QObject *parent) : MdAbstractCom(parent)
{
    QBluetoothLocalDevice localDevice;

    // Check if Bluetooth is available on this device
    if (! localDevice.isValid()) {
        qWarning() << "bluetooth not available!";
        QTimer::singleShot(500, this, SLOT(emitBtNotAvailableDeferred()));
        return;
    }

    /* 1. Step: Bluetooth LE Device Discovery */
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(5000);


    /* Device Discovery Initialization */
    connect(m_deviceDiscoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
            this, SLOT(addDevice(const QBluetoothDeviceInfo&)));
    connect(m_deviceDiscoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
            this, SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(m_deviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(deviceScanFinished()));

    startDeviceScan();
}

MdBluetoothLECom::~MdBluetoothLECom()
{    
#ifndef ANDROID
    //sigsev on exit with running bt le on android
    if ( !m_UARTService.isNull() ) delete m_UARTService;
    if ( !m_service.isNull() ) delete m_service;
    if ( !m_control.isNull() ) delete m_control;
    if ( !m_deviceDiscoveryAgent.isNull() ) delete m_deviceDiscoveryAgent;

    qDeleteAll(m_qlDevices.begin(), m_qlDevices.end() );
#endif
}
void MdBluetoothLECom::getDeviceList(QList<QString> &qlDevices){

    if(m_state == bluetoothleState::ScanFinished) {
        qlDevices = m_qlFoundDevices;
    } else {
        qlDevices.clear();
    }
}

void MdBluetoothLECom::togglePort()
{
    switch (m_state) {
        case Idle:
            openPort(); break;
        case AcquireData:
            closePort(); break;
        default:
            //sth between
            closePort();
            openPort();
    }
}

void MdBluetoothLECom::closePort() {
    if ( m_state != Idle ) {
        if ( m_control != nullptr )
            m_control->disconnectFromDevice();
        if ( m_service != nullptr ) {
            delete m_service;
            m_service = nullptr;
        }
        setState(Idle);
        emit portClosed();
    }
}

void MdBluetoothLECom::openPort() {
    if ( m_state != AcquireData ) {
        startDeviceScan();
    }
}

bool MdBluetoothLECom::setupPort(QString sport, QString speed)
{
    Q_UNUSED(sport)
    Q_UNUSED(speed)
    //not needed for BLE
    return true;
}

bool MdBluetoothLECom::changePortSettings(QString sport, QString speed)
{
    Q_UNUSED(sport)
    Q_UNUSED(speed)
    //not needed for BLE
    return true;
}



void MdBluetoothLECom::addDevice(const QBluetoothDeviceInfo &device)
{
    /* Is it a LE Bluetooth device? */
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        qWarning() << "Discovered LE Device name: " << device.name() << " Address: "
                   << device.address().toString();
        BluetoothDeviceInfo *dev = new BluetoothDeviceInfo(device);
        m_qlDevices.append(dev);
        qDebug() << "Low Energy device found. Scanning for more..." << endl;
    }

}


void MdBluetoothLECom::deviceScanFinished()
{    
    if (m_qlDevices.size() == 0) {
        qWarning() << "No Low Energy devices found" << endl;
        emit couldNotConnect2BtDevice();
    } else {
        for (int i = 0; i < m_qlDevices.size(); i++) {
           m_qlFoundDevices.append(qobject_cast<BluetoothDeviceInfo*>(m_qlDevices.at(i))->getName());
        }
        bool foundDevice = false;
        for (auto d: qAsConst(m_qlDevices)) {
            auto device = qobject_cast<BluetoothDeviceInfo *>(d);
            m_qlFoundDevices.append(device->getName());
            qDebug() << device->getName() << " address=" << device->getAddress();

            if ( device->getName().startsWith("DF1ECU") ) {
                qDebug() << "found it :)";
                foundDevice = true;
                startConnectDevice(device->getAddress());
            }
        }
        if ( !foundDevice )
            emit couldNotConnect2BtDevice();
    }    
    setState(ScanFinished);
}


void MdBluetoothLECom::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        qDebug() << "The Bluetooth adaptor is powered off, power it on before doing discovery.";
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        qDebug() << "Writing or reading from the device resulted in an error.";
    else
        qDebug() << "An unknown error has occurred.";
    emit couldNotConnect2BtDevice();
}



void MdBluetoothLECom::startDeviceScan(){

    qDeleteAll(m_qlDevices.begin(), m_qlDevices.end());
    m_qlDevices.clear();

    m_qlFoundDevices.clear();

    m_deviceDiscoveryAgent->start();
    setState(Scanning);

    qDebug()<< "Searching for low energy devices..." ;
}


void MdBluetoothLECom::startConnectDevice(const QString &address){

    m_qvMeasurements.clear();

    for (auto d: qAsConst(m_qlDevices)) {
        auto device = qobject_cast<BluetoothDeviceInfo *>(d);
        if (!device)
            continue;

        if (device->getAddress() == address )
            m_currentDevice.setDevice(device->getDevice());
    }

    if (!m_currentDevice.getDevice().isValid()) {
        qWarning() << "Not a valid device";
        emit couldNotConnect2BtDevice();
        return;
    }

    if (m_control != nullptr) {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = nullptr;

    }

    /* 2. Step: QLowEnergyController */
#ifdef ANDROID
    //sigsev deleting m_control on android on exit if there was a established BLE conn
    //-> no parent to get m_control not auto deleted
    m_control = new QLowEnergyController(m_currentDevice.getDevice());
#else
    m_control = new QLowEnergyController(m_currentDevice.getDevice(), this);
#endif
    m_control ->setRemoteAddressType(QLowEnergyController::RandomAddress);

    connect(m_control, SIGNAL(serviceDiscovered(QBluetoothUuid)),
            this, SLOT(serviceDiscovered(QBluetoothUuid)));
    connect(m_control, SIGNAL(discoveryFinished()),
            this, SLOT(serviceScanDone()));
    connect(m_control, SIGNAL(error(QLowEnergyController::Error)),
            this, SLOT(controllerError(QLowEnergyController::Error)));
    connect(m_control, SIGNAL(connected()),
            this, SLOT(deviceConnected()));
    connect(m_control, SIGNAL(disconnected()),
            this, SLOT(deviceDisconnected()));

    /* Start connecting to device */
    qDebug() << "start connecton to BLE device " << address;
    m_control->connectToDevice();
    setState(Connecting);

}

void MdBluetoothLECom::emitBtNotAvailableDeferred()
{
    emit showStatusMessage( "Bluetooth LE: not available!" );
    emit bluetoothNotAvailable();
}



void MdBluetoothLECom::serviceDiscovered(const QBluetoothUuid &gatt){

    if(gatt==QBluetoothUuid(QUuid(rn4871_sp_uuid_spp))){
        m_bFoundUARTService =true;
        qDebug() << "UART service found!";
    }
}



void MdBluetoothLECom::serviceScanDone(){

    if ( m_service != nullptr ) {
        delete m_service;
        m_service=nullptr;
    }

    if(m_bFoundUARTService){
       qDebug() << "Connecting to UART service...";
        m_service = m_control->createServiceObject(QBluetoothUuid(QUuid(rn4871_sp_uuid_spp)),this);
    }

    if(!m_service){
        qDebug() <<"UART service not found";
        emit couldNotConnect2BtDevice();
        return;
    }

    /* 3. Step: Service Discovery */
    connect(m_service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
            this, SLOT(serviceStateChanged(QLowEnergyService::ServiceState)));
    connect(m_service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
            this, SLOT(updateData(QLowEnergyCharacteristic,QByteArray)));
    connect(m_service, SIGNAL(descriptorWritten(QLowEnergyDescriptor,QByteArray)),
            this, SLOT(confirmedDescriptorWrite(QLowEnergyDescriptor,QByteArray)));

    m_service->discoverDetails();
    setState(ServiceFound);

}

void MdBluetoothLECom::deviceDisconnected()
{
    qDebug() << "UART service disconnected";
    qWarning() << "Remote device disconnected";
    emit showStatusMessage( "Bluetooth LE: disconnected" );
    closePort();

    if ( getAutoReconnect() ) {
        //try to reconnect
        openPort();
        //TODO FIXME how long will it reconnet ? TODO TEST
    }
}

void MdBluetoothLECom::deviceConnected()
{
   qDebug() << "Device connected";
   m_control->discoverServices();
   setState(Connected);
   emit portOpened();
}

void MdBluetoothLECom::controllerError(QLowEnergyController::Error error)
{
    qDebug() << "Cannot connect to remote device.";
    qWarning() << "Controller Error:" << error;
    setState(Idle);
    emit couldNotConnect2BtDevice();
}


/* Slotes for QLowEnergyService */
void MdBluetoothLECom::serviceStateChanged(QLowEnergyService::ServiceState s) {

    // A descriptoc can only be written if the service is in the ServiceDiscovered state
    switch (s) {
    case QLowEnergyService::ServiceDiscovered:
    {

        //looking for the TX characteristic
        const QLowEnergyCharacteristic TxChar = m_service->characteristic(QBluetoothUuid(QUuid(rn4871_sp_uuid_tx)));
        if (!TxChar.isValid()){
            qWarning() << "Tx characteristic not found";
            break;
        }

         //looking for the RX characteristic
        const QLowEnergyCharacteristic  RxChar = m_service->characteristic(QBluetoothUuid(QUuid(rn4871_sp_uuid_rx)));
        if (!RxChar.isValid()) {
            qWarning() << "Rx characteristic not found";
            break;
        }


       // Bluetooth LE spec Where a characteristic can be notified, a Client Characteristic Configuration descriptor
       // shall be included in that characteristic as required by the Bluetooth Core Specification
       // Tx notify is enabled
       const QLowEnergyDescriptor m_notificationDescTx = TxChar.descriptor(
                   QBluetoothUuid::ClientCharacteristicConfiguration);
        if (m_notificationDescTx.isValid()) {
            // enable notification
            m_service->writeDescriptor(m_notificationDescTx, QByteArray::fromHex("0100"));
            setState(AcquireData);
            qDebug() << "state AcquireData! :-)";
            emit showStatusMessage( "Bluetooth LE: connected" );
        }

        break;
    }
    default:
        //nothing for now
        break;
    }
}

void MdBluetoothLECom::updateData(const QLowEnergyCharacteristic &c,const QByteArray &value)
{
    // ignore any other characteristic change
    if (c.uuid() != QBluetoothUuid(QUuid(rn4871_sp_uuid_tx)))
           return;

    //qDebug() << "got Data from serial ble uart! "  << value.toHex();
    emit bytesRead(value);
}

void MdBluetoothLECom::confirmedDescriptorWrite(const QLowEnergyDescriptor &d,
                                         const QByteArray &value)
{
    if (d.isValid() && d == m_notificationDescTx && value == QByteArray("0000")) {
        //disabled notifications -> assume disconnect intent
        m_control->disconnectFromDevice();
        delete m_service;
        m_service = nullptr;
        setState(Idle);
    }
}


void MdBluetoothLECom::transmitMsg(const QString &s){
    if ( m_state == MdBluetoothLECom::AcquireData && m_service ) {
        const QLowEnergyCharacteristic  RxChar = m_service->characteristic(QBluetoothUuid(QUuid(rn4871_sp_uuid_rx)));
        qDebug()<< "transmitMsg " << s;
        QByteArray Data;
        Data.append(s);
        m_service->writeCharacteristic(RxChar, Data,QLowEnergyService::WriteWithoutResponse);
    }
}
void MdBluetoothLECom::transmitMsg(const QByteArray &ba){
    if ( m_state == MdBluetoothLECom::AcquireData && m_service ) {
        const QLowEnergyCharacteristic  RxChar = m_service->characteristic(QBluetoothUuid(QUuid(rn4871_sp_uuid_rx)));
        qDebug()<< "transmitMsg " << ba;
        m_service->writeCharacteristic(RxChar, ba,QLowEnergyService::WriteWithoutResponse);
    }
}

void MdBluetoothLECom::onReadyRead() {
    return;
}



void MdBluetoothLECom::setState(MdBluetoothLECom::bluetoothleState newState) {
    if (m_state == newState)
            return;

    qDebug() << "new state " << newState;
    m_state = newState;
    emit changedState(newState);
}

MdBluetoothLECom::bluetoothleState MdBluetoothLECom::getState() const {
    return m_state;
}
