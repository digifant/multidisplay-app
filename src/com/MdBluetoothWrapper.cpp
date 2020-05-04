#include "MdBluetoothWrapper.h"
#include "MdBluetoothLECom.h"
#include "MdBluetoothCom.h"

#include <QTimer>

MdBluetoothWrapper::MdBluetoothWrapper(QObject *parent) : MdAbstractCom(parent)
{
    wrapBtClassicAndLe();
}
MdBluetoothWrapper::~MdBluetoothWrapper() {
    m_bt.reset();
}

void MdBluetoothWrapper::togglePort()
{
    if ( m_bt != nullptr )
        m_bt->togglePort();
}

void MdBluetoothWrapper::closePort()
{
    if ( m_bt != nullptr )
        m_bt->closePort();
}
void MdBluetoothWrapper::openPort()
{
    if ( m_bt != nullptr )
        m_bt->openPort();
}
bool MdBluetoothWrapper::setupPort (QString sport, QString speed) {
    if ( m_bt != nullptr ) {
        m_bt->setupPort(sport, speed);
        return true;
    }
    return false;
}
bool MdBluetoothWrapper::changePortSettings (QString sport, QString speed) {
    if ( m_bt != nullptr ) {
        m_bt->changePortSettings(sport, speed);
        return true;
    }
    return false;
}
void MdBluetoothWrapper::transmitMsg( const QString &s) {
    if ( m_bt != nullptr )
        m_bt->transmitMsg(s);
}
void MdBluetoothWrapper::transmitMsg( const QByteArray &ba) {
    if ( m_bt != nullptr )
        m_bt->transmitMsg(ba);
}


void MdBluetoothWrapper::wrapBtClassicAndLe()
{
    switch (m_bt_version) {

        case BluetoothType::le:
            switch (m_bt_state) {
                case bluetoothState::Idle:
                    qDebug() << "BT no connection try LE";
                    m_bt.reset();
                    m_bt = std::make_shared<MdBluetoothLECom>(this);

                    connect (m_bt.get(), SIGNAL(couldNotConnect2BtDevice()), this, SLOT(couldNotConnectLE()));
                    connect (m_bt.get(), SIGNAL(bluetoothNotAvailable()), this, SLOT(bluetoothNotAvailableLE()));
                    connect (m_bt.get(), SIGNAL(portOpened()), this, SLOT(portOpenedLE()));
                    connect (m_bt.get(), SIGNAL(portClosed()), this, SLOT(portClosedLE()));
                    connect (m_bt.get(), SIGNAL(showStatusMessage ( const QString& )), this, SLOT(showStatusMessageRcv ( const QString& )));
                    connect (m_bt.get(), SIGNAL(showStatusBarSampleCount ( const QString& )), this, SLOT(showStatusBarSampleCountRcv ( const QString& )));
                    connect (m_bt.get(), SIGNAL(bytesRead(QByteArray)), this, SLOT(bytesReadRcv( const QByteArray& )) );
                    break;
                case bluetoothState::Connecting:
                    qDebug() << "BT LE is connecting";
                    break;
                case bluetoothState::AcquireData:
                    qDebug() << "BT LE is connected and running";
                    break;
            }
            break;
        case BluetoothType::classic:
            switch (m_bt_state) {
                case bluetoothState::Idle:
                    qDebug() << "BT LE no connection try BT classic";
                    m_bt.reset();
                    m_bt = std::make_shared<MdBluetoothCom>(this);
                    connect (m_bt.get(), SIGNAL(couldNotConnect2BtDevice()), this, SLOT(couldNotConnectClassic()));
                    connect (m_bt.get(), SIGNAL(bluetoothNotAvailable()), this, SLOT(bluetoothNotAvailableClassic()));
                    connect (m_bt.get(), SIGNAL(portOpened()), this, SLOT(portOpenedClassic()));
                    connect (m_bt.get(), SIGNAL(portClosed()), this, SLOT(portClosedClassic()));
                    connect (m_bt.get(), SIGNAL(showStatusMessage ( const QString& )), this, SLOT(showStatusMessageRcv ( const QString& )));
                    connect (m_bt.get(), SIGNAL(showStatusBarSampleCount ( const QString& )), this, SLOT(showStatusBarSampleCountRcv ( const QString& )));
                    connect (m_bt.get(), SIGNAL(bytesRead(QByteArray)), this, SLOT(bytesReadRcv( const QByteArray& )) );
                    break;
                case bluetoothState::Connecting:
                    qDebug() << "BT classic is connecting";
                    break;
                case bluetoothState::AcquireData:
                    qDebug() << "BT classic is connected and running";
                    break;
            }
            break;
    }
}


void MdBluetoothWrapper::couldNotConnectLE() {
    qDebug() << "BT LE: could not connect!";
    if ( m_we_got_a_connection ) {
        //should not happen!
        //Controller Error: QLowEnergyController::UnknownError
        //sufficient? or delete instance and create new one?
        m_bt->togglePort();
    } else {
        //never connected, no LE -> try classic!
        if ( ! m_bt_classic_not_available && !m_we_got_a_connection)
            m_bt_version = BluetoothType::classic;
        m_bt_state = bluetoothState::Idle;
        //wrapBtClassicAndLe();
        QTimer::singleShot(2000, this, SLOT(wrapBtClassicAndLe() ));
    }
}
void MdBluetoothWrapper::couldNotConnectClassic() {
    qDebug() << "BT classic: could not connect!";
    if ( m_we_got_a_connection ) {
        //should not happen!
    } else {
        //never connected, no LE -> try classic!
        if ( ! m_bt_le_not_available && !m_we_got_a_connection )
            m_bt_version = BluetoothType::le;
        m_bt_state = bluetoothState::Idle;
        QTimer::singleShot(2000, this, SLOT(wrapBtClassicAndLe() ));
        //wrapBtClassicAndLe();
    }
}
void MdBluetoothWrapper::bluetoothNotAvailableLE() {    
    qDebug() << "BT LE not available!";
    if ( m_we_got_a_connection ) {
        //should not happen!
    } else {
        //never connected, no LE -> try classic!
        m_bt_version = BluetoothType::classic;
        m_bt_state = bluetoothState::Idle;
        m_bt_le_not_available = true;
        if ( ! m_bt_classic_not_available )
            QTimer::singleShot(2000, this, SLOT(wrapBtClassicAndLe() ));
        else
            qDebug() << "BT classic AND LE not available";
    }
}
void MdBluetoothWrapper::bluetoothNotAvailableClassic() {
    qDebug() << "BT classic not available!";
    if ( m_we_got_a_connection ) {
        //should not happen!
    } else {
        //never connected, no LE -> try classic!
        m_bt_version = BluetoothType::le;
        m_bt_state = bluetoothState::Idle;
        m_bt_classic_not_available = true;
        if ( ! m_bt_le_not_available )
            QTimer::singleShot(2000, this, SLOT(wrapBtClassicAndLe() ));
        else
            qDebug() << "BT classic AND LE not available";
    }

}
void MdBluetoothWrapper::portClosedLE() {
    m_bt_state = Idle;
    emit portClosed();
    if ( ! m_bt->getAutoReconnect() )
        wrapBtClassicAndLe();
}
void MdBluetoothWrapper::portClosedClassic() {
    m_bt_state = Idle;
    emit portClosed();
    if ( ! m_bt->getAutoReconnect() )
        wrapBtClassicAndLe();
}
void MdBluetoothWrapper::portOpenedLE() {
    m_we_got_a_connection = true;
    m_bt_state = AcquireData;
    m_bt->setAutoReconnect(true);
    emit portOpened();
}
void MdBluetoothWrapper::portOpenedClassic() {
    m_we_got_a_connection = true;
    m_bt_state = AcquireData;
    m_bt->setAutoReconnect(true);
    emit portOpened();
}

void MdBluetoothWrapper::onReadyRead()
{
    //wrapper dont needs this
    return;
}

void MdBluetoothWrapper::showStatusMessageRcv ( const QString& s) {
    emit showStatusMessage(s);
}
void MdBluetoothWrapper::showStatusBarSampleCountRcv ( const QString& s) {
    emit showStatusBarSampleCount(s);
}

void MdBluetoothWrapper::bytesReadRcv(const QByteArray &ba)
{
    emit bytesRead ( ba );
}
