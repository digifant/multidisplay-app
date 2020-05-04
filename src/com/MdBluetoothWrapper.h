#ifndef MDBLUETOOTHWRAPPER_H
#define MDBLUETOOTHWRAPPER_H

#include <com/MdAbstractCom.h>
#include <com/MdBluetoothCom.h>
#include <com/MdBluetoothLECom.h>

#include<memory>

/**
 * @brief The MdBluetoothWrapper class tries alternating to connect bluetooth classic and LE
 * attention: bluez on linux caches discovered devices ; they get discovered even if the are offline! :(
 * FIXME maybe we have more than one devices in our list with first one offline!
 * TODO: it tries connecting infinitely; should we limit it?
 */
class MdBluetoothWrapper : public MdAbstractCom
{
    Q_OBJECT
public:
    MdBluetoothWrapper(QObject* parent);
    ~MdBluetoothWrapper();

    enum BluetoothType {
           classic = 0,
           le,
    };
    Q_ENUM(BluetoothType)
    enum bluetoothState {
           Idle = 0,
           Connecting,
           AcquireData
    };
    Q_ENUM(bluetoothState)



signals:
    void showStatusMessage ( const QString& );
    void showStatusBarSampleCount ( const QString& );

    void portOpened();
    void portClosed();

public slots:
    virtual void togglePort();
    virtual void closePort();
    virtual void openPort();

    virtual bool setupPort (QString sport="/dev/ttyUSB0", QString speed="115200");
    virtual bool changePortSettings (QString sport="/dev/ttyUSB0", QString speed="115200");

    virtual void transmitMsg( const QString &s);
    virtual void transmitMsg( const QByteArray &ba);

    void showStatusMessageRcv ( const QString& );
    void showStatusBarSampleCountRcv ( const QString& );
    void bytesReadRcv ( const QByteArray & );

    //! wrapper stuff
    void couldNotConnectLE();
    void couldNotConnectClassic();
    void bluetoothNotAvailableLE();
    void bluetoothNotAvailableClassic();
    void portClosedLE();
    void portClosedClassic();
    void portOpenedLE();
    void portOpenedClassic();

protected slots:
    virtual void onReadyRead();
    void wrapBtClassicAndLe();

protected:

private:
    //! start discovery with BT classic or LE?
    BluetoothType m_bt_version = BluetoothType::le;
    //BluetoothType m_bt_version = BluetoothType::classic;
    bluetoothState m_bt_state = bluetoothState::Idle;
    std::shared_ptr<MdAbstractCom> m_bt = nullptr;
    bool m_we_got_a_connection = false;
    bool m_bt_le_not_available = false;
    bool m_bt_classic_not_available = false;
};

#endif // MDBLUETOOTHWRAPPER_H
