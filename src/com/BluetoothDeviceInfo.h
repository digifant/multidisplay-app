#ifndef BLUETOOTHDEVICEINFO_H
#define BLUETOOTHDEVICEINFO_H

#include <QObject>
#include <QBluetoothDeviceInfo>
#include <qbluetoothaddress.h>
#include <QList>

class BluetoothDeviceInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceName READ getName NOTIFY deviceChanged)
    Q_PROPERTY(QString deviceAddress READ getAddress NOTIFY deviceChanged)
public:
    BluetoothDeviceInfo() = default;
    BluetoothDeviceInfo(const QBluetoothDeviceInfo &d);
    QString getAddress() const;
    QString getName() const;
    QBluetoothDeviceInfo getDevice();
    void setDevice(const QBluetoothDeviceInfo &dev);

Q_SIGNALS:
    void deviceChanged();

private:
    QBluetoothDeviceInfo device;


};

#endif // BLUETOOTHDEVICEINFO_H
