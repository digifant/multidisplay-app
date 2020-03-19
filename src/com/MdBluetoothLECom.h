#ifndef MDBLUETOOTHLECOM_H
#define MDBLUETOOTHLECOM_H

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothServiceDiscoveryAgent>
#include <QPointer>
#include <QLowEnergyController>
#include <com/MdAbstractCom.h>
#include <com/BluetoothDeviceInfo.h>


class MdBluetoothLECom : public MdAbstractCom
{
    Q_OBJECT
public:
    /**
     * @brief MdBluetoothLECom bluetooth LE
     * starts service discovery and connects to spp service if service is found
     * @param parent
     */
    explicit MdBluetoothLECom(QObject *parent = 0);
    virtual ~MdBluetoothLECom ();

    enum bluetoothleState {
           Idle = 0,
           Scanning,
           ScanFinished,
           Connecting,
           Connected,
           ServiceFound,
           AcquireData
    };
    Q_ENUM(bluetoothleState)

    void setState(MdBluetoothLECom::bluetoothleState newState);
    MdBluetoothLECom::bluetoothleState getState() const;
    void getDeviceList(QList<QString> &qlDevices);


public slots:
    void togglePort();
    void closePort();
    void openPort();

    bool setupPort (QString sport="/dev/ttyUSB0", QString speed="115200");
    bool changePortSettings (QString sport="/dev/ttyUSB0", QString speed="115200");

    void transmitMsg(const QString &s);
    void transmitMsg( const QByteArray &ba);

protected slots:
    //! not used for BLE. there's no socket!
    void onReadyRead();

private slots:
    /* Slots for QBluetothDeviceDiscoveryAgent */
    void addDevice(const QBluetoothDeviceInfo&);
    void deviceScanFinished();
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error);

    /* Slots for QLowEnergyController */
    void serviceDiscovered(const QBluetoothUuid &);
    void serviceScanDone();
    void controllerError(QLowEnergyController::Error);
    void deviceConnected();
    void deviceDisconnected();

    /* Slots for QLowEnergyService */
    void serviceStateChanged(QLowEnergyService::ServiceState s);
    void updateData(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void confirmedDescriptorWrite(const QLowEnergyDescriptor &d, const QByteArray &value);

    /* Slots for user */
    void startDeviceScan();
    void startConnectDevice(const QString &address);

    void emitBtNotAvailableDeferred();


signals:
    void showStatusMessage ( const QString& );
    void showStatusBarSampleCount ( const QString& );

    void portOpened();
    void portClosed();

    //! new data received
    void bytesRead ( const QByteArray & );

    /* Signals for user */
    void changedState(MdBluetoothLECom::bluetoothleState newState);

private:    
    BluetoothDeviceInfo m_currentDevice;
    QPointer<QBluetoothDeviceDiscoveryAgent> m_deviceDiscoveryAgent = nullptr;
    QList< QPointer<QObject> > m_qlDevices;
    QList<QString> m_qlFoundDevices;
    QVector<quint16> m_qvMeasurements;
    QPointer<QLowEnergyController> m_control = nullptr;
    QPointer<QLowEnergyService> m_service = nullptr;
    QLowEnergyDescriptor m_notificationDescTx;
    QPointer<QLowEnergyService> m_UARTService = nullptr;
    bool m_bFoundUARTService;

    MdBluetoothLECom::bluetoothleState m_state = bluetoothleState::Idle;


    QString rn4871_sp_uuid_spp = "49535343-fe7d-4ae5-8fa9-9fafd205e455";
    QString rn4871_sp_uuid_tx = "49535343-1e4d-4bd9-ba61-23c647249616";
    QString rn4871_sp_uuid_rx = "49535343-8841-43F4-A8D4-ECBE34729BB3";
};

#endif // MDBLUETOOTHLECOM_H
