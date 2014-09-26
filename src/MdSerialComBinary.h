#ifndef MDSERIALCOMBINARY_H
#define MDSERIALCOMBINARY_H

#include <MdSerialCom.h>
#include <QTime>

#define MD_FRAMEBEGIN 2
#define MD_FRAMEEND 3
#define MD_STATUS_RECEIVING 1
#define MD_STATUS_FRAME_COMPLETE 2
#define MD_STATUS_FRAMEERROR 3
#define MD_STATUS_WAITING_FOR_TAG 4

#define MD_MAXFRAME_SIZE 97
//87 bytes MD2 data plus digifant data
//#define MD_SERIALOUT_BINARY_TAG 88
//#define MD_SERIALOUT_BINARY_TAG 92
//#define MD_SERIALOUT_BINARY_TAG 95
#define MD_SERIALOUT_BINARY_TAG 95
#define MD_SERIALOUT_BINARY_BOOSTPID_TAG 69
#define MD_SERIALOUT_BINARY_CONFIG_TAG 99
#define MD_SERIALOUT_BINARY_TAG_GEAR_RATIO_6G 17

//STX tag=24 gearX mode serial 16 bytes map ETX
#define MD_SERIALOUT_BINARY_TAG_N75_DUTY_MAP 22
#define MD_SERIALOUT_BINARY_TAG_N75_SETPOINT_MAP 38
#define MD_SERIALOUT_BINARY_TAG_ACK 4
#define MD_SERIALOUT_BINARY_TAG_N75_PARAMS 23

class MdData;
class Map16x1_NTC_ECT;
class Map16x1_NTC_IAT;
class Map16x1_Voltage;

class MdSerialComBinary : public MdSerialCom
{
    Q_OBJECT
public:
    MdSerialComBinary(MdData *data);
    ~MdSerialComBinary();

signals:
    //! remember to free the data after use!
    void n75DutyMapreceived (quint8 gear, quint8 mode, quint8 serial, QVector<quint8> *data);
    void n75SetpointMapreceived (quint8 gear, quint8 mode, quint8 serial, QVector<double> *data);
    void ackReceived (quint8 serial);
    void n75SettingsReceived (quint8 serial, double aKp, double aKi, double aKd,
                              double cKp, double cKi, double cKd,
                              double aAT, double cAT, bool pid_enabled,
                              double maxBoost);
    void gearboxSettingsReceived (quint8 serial, double g1, double g2, double g3, double g4, double g5, double g6);
    //! signals that an complete frame was received
    void frameReceived();

public slots:
    void mdCmdReqN75DutyMap (quint8 gear, quint8 mode, quint8 serial);
    void mdCmdReqN75SetpointMap (quint8 gear, quint8 mode, quint8 serial);
    void mdCmdWriteN75DutyMap (quint8 gear, quint8 mode, quint8 serial, QVector<quint8>* data);
    void mdCmdWriteN75SetpointMap (quint8 gear, quint8 mode, quint8 serial, QVector<double>* data);
    void mdCmdWriteN75MapsToEEprom ();
    void mdCmdLoadN75MapsFromEEprom ();

    void mdCmdReqN75Settings (quint8 serial=0);
    void mdCmdWriteN75Settings (quint8 serial, double aKp, double aKi, double aKd,
                                double cKp, double cKi, double cKd,
                                double aAT, double cAT, bool pid_enabled, double maxBoost );
    void mdCmdReadN75SettingsFromEEprom (quint8 serial=0);
    void mdCmdWriteN75SettingsToEEprom (quint8 serial=0);

    void mdSendCommand (quint8 cmd, quint8 subcmd, quint8 serial=0);
    void mdCmdSetSerialFrequency (quint16 frequency, quint8 serial=0);

    void mdCmdReadGearbox (quint8 serial=0);
    void mdCmdWriteGearbox (double g1, double g2, double g3, double g4, double g5, double g6, quint8 serial=0);

private slots:
    virtual void onReadyRead();

protected slots:
    void debugDataGenUpdate();

protected:
    qint8 index;
    qint8 status;
    qint16 discarded_frames;
    quint8 framelength;
    bool df_connected;

    union {
            quint8 asBytes[MD_MAXFRAME_SIZE];
        } rcvData;
    QByteArray *sdata;

    void convertReceivedFrame();
    void convertReceivedMd2Frame();
    void convertReceivedN75DutyMapFrame();
    void convertReceivedN75SetpointMapFrame();
    void convertReceivedN75SettingsFrame();
    void convertGearBoxFrame();

    double inline fixed_b100_2double (quint16 in);
    quint16 inline double2_fixed_b100 (double in);
    QByteArray inline double2_fixed_b100_Ba (double in);
    double inline fixed_b1000_2double (quint16 in);
    quint16 inline double2_fixed_b1000 (double in);
    QByteArray inline double2_fixed_b1000_Ba (double in);

    Map16x1_NTC_ECT *dfEctMap;
    Map16x1_NTC_IAT *dfIatMap;
    Map16x1_Voltage *dfVoltageMap;

    QTime timeHelper;
    QTime freqMeasure;
};

#endif // MDSERIALCOMBINARY_H
