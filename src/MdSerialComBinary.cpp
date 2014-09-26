#include "MdSerialComBinary.h"
#include "MdData.h"
#include "Map16x1.h"
#include "qextserialport.h"
#include <QDebug>
#include <AppEngine.h>
#include <QTime>
#include <QTimer>
#include <QSettings>

MdSerialComBinary::MdSerialComBinary(MdData *data) :
        MdSerialCom(data),
        index(0),
        status(MD_STATUS_FRAME_COMPLETE),
        discarded_frames(0),
        framelength(0),
        df_connected(false)

{
    sdata = new QByteArray();
    sdata->resize(MD_MAXFRAME_SIZE);
    dfEctMap = new Map16x1_NTC_ECT();
    dfIatMap = new Map16x1_NTC_IAT();
    dfVoltageMap = new Map16x1_Voltage();
    timeHelper = QTime::currentTime();
    timeHelper.start();
    freqMeasure = QTime::currentTime();
    freqMeasure.start();

    QSettings settings("MultiDisplay", "UI");
    if ( settings.value("debug/generate_data", QVariant(false)).toBool() ) {
        debugDataGenTimer = new QTimer(this);
        connect ( debugDataGenTimer, SIGNAL(timeout()), this, SLOT(debugDataGenUpdate()) );
        debugDataGenTimer->start (1000);
        debugRPMCounter=0;
        debugTime = 0;
    }
}

MdSerialComBinary::~MdSerialComBinary() {
    if ( dfEctMap )
        delete ( dfEctMap );
    if ( dfIatMap )
        delete ( dfIatMap );
}

void MdSerialComBinary::onReadyRead() {
    QByteArray bytes;
    int bc = port->bytesAvailable();
    bytes.resize(bc);
    port->read(bytes.data(), bytes.size());
    emit bytesRead ( bytes );

    for ( qint32 i = 0 ; i < bc ; i++ ) {
        quint8 d = (quint8) bytes[i];

        switch ( status ) {

        case MD_STATUS_FRAMEERROR:
        case MD_STATUS_FRAME_COMPLETE:
            //new frame --> check for start char
            index = 0;
            for ( int i=0; i < (MD_MAXFRAME_SIZE-1) ; i++ ) {
                    rcvData.asBytes[i]=0xFF;
                    (*sdata)[i]=0xFF;
            }

            if ( d != MD_FRAMEBEGIN ) {
                    //skip it
            } else {
                    status = MD_STATUS_WAITING_FOR_TAG;
                    rcvData.asBytes[index] = d;
                    (*sdata)[index]=d;
                    index++;
            }
            break;

        case MD_STATUS_WAITING_FOR_TAG:
            if ( d == MD_SERIALOUT_BINARY_TAG || d == MD_SERIALOUT_BINARY_BOOSTPID_TAG
                 || d == MD_SERIALOUT_BINARY_TAG_N75_DUTY_MAP || d == MD_SERIALOUT_BINARY_TAG_N75_SETPOINT_MAP
                 || d == MD_SERIALOUT_BINARY_TAG_ACK  || d == MD_SERIALOUT_BINARY_TAG_N75_PARAMS
                 || d == MD_SERIALOUT_BINARY_TAG_GEAR_RATIO_6G ) {
                status = MD_STATUS_RECEIVING;
                framelength = d;
                rcvData.asBytes[index] = d;
                (*sdata)[index]=d;
                index++;
            } else {
                qDebug() << "(WARN) expected tag but did not get one! d=" << d;
                framelength = 0;
            }
            break;

        case MD_STATUS_RECEIVING:
            if ( index == framelength-1 ) {
                    //last char -> check for end char
                    if ( d != MD_FRAMEEND ) {
                            discarded_frames++;
                            if ( discarded_frames % 100 == 0 )
                                qDebug() << "(WARN) frame discarded! expected framelength=" << framelength << " #discarded frames=" << discarded_frames << " d=" << d << " data=" << sdata->toHex();
                            status = MD_STATUS_FRAMEERROR;
                    } else {
                            rcvData.asBytes[index] = d;
                            (*sdata)[index]=d;
                            //frame complete!
                            status = MD_STATUS_FRAME_COMPLETE;
                            index = 0;
                            //do sth with it!
                            emit frameReceived();
                            this->convertReceivedFrame();

                    }
            } else {
                    rcvData.asBytes[index] = d;
                    (*sdata)[index]=d;
                    index++;
            }
            break;
        }
    }

}

void MdSerialComBinary::convertReceivedFrame() {
//    qDebug() << "frame competely received! tag=" << rcvData.asBytes[1] << "(" << sdata->toHex() << ")";

    switch  ( rcvData.asBytes[1] ) {
    case MD_SERIALOUT_BINARY_TAG:
        convertReceivedMd2Frame();
        break;
    case MD_SERIALOUT_BINARY_BOOSTPID_TAG:
        break;
    case MD_SERIALOUT_BINARY_TAG_N75_DUTY_MAP:
        convertReceivedN75DutyMapFrame();
        break;
    case MD_SERIALOUT_BINARY_TAG_N75_SETPOINT_MAP:
        convertReceivedN75SetpointMapFrame();
        break;
    case MD_SERIALOUT_BINARY_TAG_ACK:
        emit ackReceived (rcvData.asBytes[2]);
        emit showStatusMessage("ack serial " + rcvData.asBytes[2]);
        qDebug() << "ACK received serial=" << rcvData.asBytes[2];
        break;
    case MD_SERIALOUT_BINARY_TAG_N75_PARAMS:
        convertReceivedN75SettingsFrame();
        break;
    case MD_SERIALOUT_BINARY_TAG_GEAR_RATIO_6G:
        convertGearBoxFrame();
        break;
    default:
        qDebug() << "handling of tag " << rcvData.asBytes[1] << " not yet implemented!";
    }

}


double MdSerialComBinary::fixed_b100_2double (quint16 in) {
        return (double) (in / 100.0);
}
quint16 MdSerialComBinary::double2_fixed_b100 (double in) {
    return (quint16) (in * 100);
}
double MdSerialComBinary::fixed_b1000_2double (quint16 in) {
        return (double) (in / 1000.0);
}
quint16 MdSerialComBinary::double2_fixed_b1000 (double in) {
    return (quint16) (in * 1000);
}
QByteArray MdSerialComBinary::double2_fixed_b100_Ba (double in) {
    QByteArray t;
    t.push_back( quint8 (double2_fixed_b100( in ) & 0xFF) );
    t.push_back( quint8 (double2_fixed_b100( in ) >> 8) );
    return t;
}
QByteArray MdSerialComBinary::double2_fixed_b1000_Ba (double in) {
    QByteArray t;
    t.push_back( quint8 (double2_fixed_b1000( in ) & 0xFF) );
    t.push_back( quint8 (double2_fixed_b1000( in ) >> 8) );
    return t;
}


void MdSerialComBinary::convertReceivedN75DutyMapFrame() {
    //STX tag=24 gearX mode serial 16 bytes map ETX
    quint8 gear = rcvData.asBytes[2];
    quint8 mode = rcvData.asBytes[3];
    quint8 serial = rcvData.asBytes[4];
    qDebug() << "convertReceivedN75DutyMapFrame n75 map mode=" << mode << " gear=" << gear << " serial=" << serial;
    QVector<quint8> *a = new QVector<quint8>();
    for ( quint8 i = 0 ; i < 16 ; i++ )
        a->append(rcvData.asBytes[5+i]);
    qDebug() << *a;
    emit n75DutyMapreceived (gear, mode, serial, a);
}
void MdSerialComBinary::convertReceivedN75SetpointMapFrame() {
    //STX tag=24 gearX mode serial 16 bytes map ETX
    quint8 gear = rcvData.asBytes[2];
    quint8 mode = rcvData.asBytes[3];
    quint8 serial = rcvData.asBytes[4];
    qDebug() << "convertReceivedN75SetpointMapFrame() n75 map mode=" << mode << " gear=" << gear << " serial=" << serial;
    QVector<double> *a = new QVector<double>();
    for ( quint8 i = 0 ; i < 32 ; i=i+2 ) {
        quint16 n = (rcvData.asBytes[5+i+1] << 8) + rcvData.asBytes[5+i];
        a->append( fixed_b100_2double(n) );
    }
    qDebug() << *a;
    emit n75SetpointMapreceived (gear, mode, serial, a);
}
void MdSerialComBinary::convertReceivedN75SettingsFrame() {
    //STX tag=21 serial aKp aKi aKd cKp cKi cKd aAT cAT (16bit fixed uint16 base 100) flags (uint8 bit0=pid enable) ETX
    quint8 serial = rcvData.asBytes[2];
    double aKp = fixed_b100_2double( (quint16) (rcvData.asBytes[4] << 8) + rcvData.asBytes[3] );
    double aKi = fixed_b100_2double( (quint16) (rcvData.asBytes[6] << 8) + rcvData.asBytes[5] );
    double aKd = fixed_b100_2double( (quint16) (rcvData.asBytes[8] << 8) + rcvData.asBytes[7] );
    double cKp = fixed_b100_2double( (quint16) (rcvData.asBytes[10] << 8) + rcvData.asBytes[9] );
    double cKi = fixed_b100_2double( (quint16) (rcvData.asBytes[12] << 8) + rcvData.asBytes[11] );
    double cKd = fixed_b100_2double( (quint16) (rcvData.asBytes[14] << 8) + rcvData.asBytes[13] );
    double aAT = fixed_b100_2double( (quint16) (rcvData.asBytes[16] << 8) + rcvData.asBytes[15] );
    double cAT = fixed_b100_2double( (quint16) (rcvData.asBytes[18] << 8) + rcvData.asBytes[17] );
    quint8 flags = rcvData.asBytes[19];
    bool pid_enabled = false;
    if ( flags & 1 )
        pid_enabled = true;
    double maxBoost = fixed_b100_2double( (quint16) (rcvData.asBytes[21] << 8) + rcvData.asBytes[20] );
    emit n75SettingsReceived(serial, aKp, aKi, aKd, cKp, cKi, cKd, aAT, cAT, pid_enabled, maxBoost );
}
void MdSerialComBinary::convertGearBoxFrame() {
    //STX tag=17 serial gears gear*uint16(fixed_int_base1000) ETX
    qDebug() << "convertGearBoxFrame()";
    quint8 serial = rcvData.asBytes[2];
    quint8 gear = rcvData.asBytes[3];
    double g1 = fixed_b1000_2double( (quint16) (rcvData.asBytes[5] << 8) + rcvData.asBytes[4] );
    double g2 = fixed_b1000_2double( (quint16) (rcvData.asBytes[7] << 8) + rcvData.asBytes[6] );
    double g3 = fixed_b1000_2double( (quint16) (rcvData.asBytes[9] << 8) + rcvData.asBytes[8] );
    double g4 = fixed_b1000_2double( (quint16) (rcvData.asBytes[11] << 8) + rcvData.asBytes[10] );
    double g5 = fixed_b1000_2double( (quint16) (rcvData.asBytes[13] << 8) + rcvData.asBytes[12] );
    double g6 = fixed_b1000_2double( (quint16) (rcvData.asBytes[15] << 8) + rcvData.asBytes[14] );
    emit gearboxSettingsReceived(serial, g1, g2, g3, g4, g5, g6);
}

void MdSerialComBinary::convertReceivedMd2Frame() {
    int millisElapsed = freqMeasure.restart();

    int base = 2;

    quint32 time = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8) + (rcvData.asBytes[base+2] << 16) + (rcvData.asBytes[base+3] << 24);
    base += 4;

    quint16 rpm = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;

    //absolute boost!
    double  boost = fixed_b100_2double( quint16 ( rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8) ) ) - 1;
    base +=2;       

    quint8 throttle = rcvData.asBytes[base+0];
    base +=1;


    double  lambda = fixed_b100_2double( quint16 ( rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8) ) );
    base +=2;

    double  lmm = fixed_b100_2double( quint16 ( rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8) ) );
    base +=2;

    double  casetemp = fixed_b100_2double( quint16 ( rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8) ) );
    base +=2;

    quint16 egt0 = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;
    quint16 egt1 = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;
    quint16 egt2 = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;
    quint16 egt3 = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;
    quint16 egt4 = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;
    quint16 egt5 = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;
    quint16 egt6 = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;
    quint16 egt7 = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;

    double  batVolt = fixed_b100_2double( quint16 ( rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8) ) );
    base +=2;

    //FIXME pressure / temp ints???
    quint16 vdo_pres1 = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;
    quint16 vdo_pres2 = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;
    quint16 vdo_pres3 = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;
    quint16 vdo_temp1 = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;
    quint16 vdo_temp2 = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;
    quint16 vdo_temp3 = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;

    //quint16 speed = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    double  speed = fixed_b100_2double( quint16 ( rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8) ) );
    base +=2;

    quint8 gear = rcvData.asBytes[base+0];
    base +=1;
    quint8 n75 = rcvData.asBytes[base+0];
    base +=1;
    double  n75_req_boost = fixed_b100_2double( quint16 ( rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8) ) );
    base +=2;
    quint8 n75_req_boost_pwm = rcvData.asBytes[base+0];
    base++;
    quint8 flags = rcvData.asBytes[base+0];
    base +=1;
    double efr_speed_tmp = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    double efr_speed = 0;
    if ( efr_speed_tmp == 0xFFFF )
            efr_speed = 0;
        else
            efr_speed = 40000000 / (efr_speed_tmp);

    base +=2;
    double knock = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base +=2;

//    qDebug() << "efr_speed raw=" << efr_speed_tmp << " usec rise2fall=" << efr_speed_tmp/2 << " usec rise2rise "
//             << efr_speed_tmp << " freq=" << 1000000/efr_speed_tmp << "Hz speed=" << efr_speed << " RPM"
//             << " DataOut " << ((millisElapsed > 0) ? 1000/millisElapsed : -1) << " Hz";

#if  defined (Q_WS_MAEMO_5)  || defined (ANDROID)
    ;
#else
    qDebug() << " DataOut " << ((millisElapsed > 0) ? 1000/millisElapsed : -1) << " Hz";
#endif

    // digifant

    //remember: 68HC11 is big endian!!!
    //avr8 is little endian
    quint8 df_boost_raw = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_lambda = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_raw_knock = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_ect_raw = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_iat_raw = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_co_poti = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_flags = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_ign_raw = rcvData.asBytes[base+0];
    base +=1;
    //changed 2013-9-18
    quint8 df_rpm_map=0;
    quint8 df_lc_flags = rcvData.asBytes[base+0];
    //quint8 df_rpm_map = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_cyl1_knock_retard = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_cyl1_knock_decay = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_cyl2_knock_retard = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_cyl2_knock_decay = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_cyl3_knock_retard = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_cyl3_knock_decay = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_cyl4_knock_retard = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_cyl4_knock_decay = rcvData.asBytes[base+0];
    base +=1;
    //removed 2013-9-18
    quint8 df_sci_counter=0;
    //quint8 df_sci_counter = rcvData.asBytes[base+0];
    //base +=1;
    quint8 df_voltage_raw = rcvData.asBytes[base+0];
    base +=1;
    quint16 df_inj_time = ((rcvData.asBytes[base+0] << 8) + (rcvData.asBytes[base+1]) )*2;
    base +=2;
    quint8 df_cold_startup_enrichment = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_warm_startup_enrichment = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_ect_enrichment = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_acceleration_enrichment = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_counter_startup_enrichment = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_iat_enrichment = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_ignition_addon_counter = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_igniton_addon = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_ect_injection_addon = rcvData.asBytes[base+0];
    base +=1;
    quint8 df_isv = rcvData.asBytes[base+0];
    base += 1;
    quint16 df_rpm_delta_hall = (rcvData.asBytes[base+0] << 8) + (rcvData.asBytes[base+1]);
    base +=2;
    double df_computed_rpm = 0;
    if ( df_rpm_delta_hall > 0 )
        df_computed_rpm = 30000000 / df_rpm_delta_hall;

    //from avr little endian!
    quint16 df_freq = rcvData.asBytes[base+0] + (rcvData.asBytes[base+1] << 8);
    base += 2;
    quint8 df_active_frame = rcvData.asBytes[base+0];
    base += 1;
    if ( df_active_frame < 255 )
        df_connected = true;

    double df_ignition_total_retard = ( df_cyl1_knock_retard + df_cyl2_knock_retard + df_cyl3_knock_retard + df_cyl4_knock_retard ) * 0.351563 ;
    double df_ect = dfEctMap->mapValue( df_ect_raw );
    double df_iat = dfIatMap->mapValue( df_iat_raw );
    double df_ignition = (df_ign_raw *-0.351563)+73.9;
    double df_voltage = dfVoltageMap->mapValue(df_voltage_raw);


#if  defined (Q_WS_MAEMO_5)  || defined (ANDROID)
    ;
#else
    if ( ! df_connected ) {
        if ( timeHelper.elapsed() > 10000 ) {
            qDebug() << "no connection to Digifant";
            timeHelper.restart();
        }
    } else {
        if ( timeHelper.elapsed() > 1000 ) {
            qDebug() << "K-Line frame#=" << df_active_frame << " | DF K-Line freq=" << df_freq << " | df_computed_rpm=" << df_computed_rpm;
            timeHelper.restart();
        }

    }
#endif


    MdSensorRecord *sr = new MdSensorRecord ( time, rpm, throttle, boost, lambda, lmm,
                                              casetemp, egt0, egt1, egt2, egt3, egt4, egt5, egt6, egt7,
                                              batVolt,
                                              vdo_pres1, vdo_pres2, vdo_pres3,
                                              vdo_temp1, vdo_temp2, vdo_temp3,
                                              speed, gear, n75, n75_req_boost, n75_req_boost_pwm, flags,
                                              efr_speed,
                                              df_boost_raw, df_lambda, df_raw_knock, df_ect_raw, df_iat_raw, df_co_poti, df_flags, df_ign_raw,
                                              df_cyl1_knock_retard, df_cyl1_knock_decay, df_cyl2_knock_retard, df_cyl2_knock_decay,
                                              df_cyl3_knock_retard, df_cyl3_knock_decay, df_cyl4_knock_retard, df_cyl4_knock_decay,
                                              df_voltage_raw, df_inj_time, df_cold_startup_enrichment, df_warm_startup_enrichment,
                                              df_ect_enrichment, df_acceleration_enrichment, df_counter_startup_enrichment, df_iat_enrichment,
                                              df_ignition_addon_counter, df_igniton_addon, df_ect_injection_addon,
                                              df_rpm_delta_hall, df_isv, df_lc_flags,
                                              df_ignition_total_retard, df_ect, df_iat, df_ignition, df_voltage,
                                              knock, df_freq, df_active_frame );
    data->addDataRecord ( new MdDataRecord (sr), AppEngine::getInstance()->getActualizeVis1() );

}







void MdSerialComBinary::mdCmdReqN75DutyMap (quint8 gear, quint8 mode, quint8 serial) {
    QByteArray t;
    t.push_back ( (quint8) 6 );
    t.push_back ( (quint8) 1 );
    t.push_back ( (quint8) gear );
    t.push_back ( (quint8) mode );
    t.push_back ( (quint8) serial );
    qDebug() << "cmd transmit length=" << t.length() << " " << t.toHex();
    transmitMsg(t);
}
void MdSerialComBinary::mdCmdReqN75SetpointMap (quint8 gear, quint8 mode, quint8 serial) {
    QByteArray t;
    t.push_back ( (quint8) 6 );
    t.push_back ( (quint8) 2 );
    t.push_back ( (quint8) gear );
    t.push_back ( (quint8) mode );
    t.push_back ( (quint8) serial );
    qDebug() << "cmd transmit length=" << t.length() << " " << t.toHex();
    transmitMsg(t);
}
void MdSerialComBinary::mdCmdWriteN75DutyMap (quint8 gear, quint8 mode, quint8 serial, QVector<quint8>* data) {
    QByteArray t;
    t.push_back ( (quint8) 6 );
    t.push_back ( (quint8) 3 );
    t.push_back ( (quint8) gear );
    t.push_back ( (quint8) mode );
    t.push_back ( (quint8) serial );
    for (quint8 i = 0 ; i < 16 ; i++)
        t.push_back( data->at(i));
    qDebug() << "cmd transmit length=" << t.length() << " " << t.toHex();
    transmitMsg(t);
    delete data;
}

void MdSerialComBinary::mdCmdWriteN75SetpointMap (quint8 gear, quint8 mode, quint8 serial, QVector<double>* data) {
    QByteArray t;
    t.push_back ( (quint8) 6 );
    t.push_back ( (quint8) 4 );
    t.push_back ( (quint8) gear );
    t.push_back ( (quint8) mode );
    t.push_back ( (quint8) serial );
    for (quint8 i = 0 ; i < 16 ; i++) {
        t.push_back( quint8 (double2_fixed_b100( data->at(i) ) & 0xFF) );
        t.push_back( quint8 (double2_fixed_b100( data->at(i) ) >> 8) );
    }
    qDebug() << "cmd transmit length=" << t.length() << " " << t.toHex();
    transmitMsg(t);
    delete data;
}
void MdSerialComBinary::mdCmdWriteN75MapsToEEprom () {
    mdSendCommand (6, 6, 6);
}
void MdSerialComBinary::mdCmdLoadN75MapsFromEEprom () {
    mdSendCommand (6, 5, 7);
}


void MdSerialComBinary::mdCmdReqN75Settings (quint8 serial) {
    mdSendCommand (6, 10, serial);
}
void MdSerialComBinary::mdCmdWriteN75Settings (quint8 serial, double aKp, double aKi, double aKd,
                            double cKp, double cKi, double cKd,
                            double aAT, double cAT, bool pid_enabled, double maxBoost ) {
    QByteArray t;
    t.push_back ( (quint8) 6 );
    t.push_back ( (quint8) 9 );
    t.push_back ( (quint8) serial );
    t.push_back ( double2_fixed_b100_Ba  (aKp) );
    t.push_back ( double2_fixed_b100_Ba  (aKi) );
    t.push_back ( double2_fixed_b100_Ba  (aKd) );
    t.push_back ( double2_fixed_b100_Ba  (cKp) );
    t.push_back ( double2_fixed_b100_Ba  (cKi) );
    t.push_back ( double2_fixed_b100_Ba  (cKd) );
    t.push_back ( double2_fixed_b100_Ba  (aAT) );
    t.push_back ( double2_fixed_b100_Ba  (cAT) );
    quint8 flags = 0;
    if ( pid_enabled )
        flags |= 1;
    t.push_back ( flags );
    t.push_back ( double2_fixed_b100_Ba  (maxBoost) );
    qDebug() << "mdCmdWriteN75Settings tx length=" << t.length() << " data=" << t.toHex();
    transmitMsg(t);
}

void MdSerialComBinary::mdCmdReadN75SettingsFromEEprom (quint8 serial) {
    mdSendCommand (6, 7, serial);
}
void MdSerialComBinary::mdCmdWriteN75SettingsToEEprom (quint8 serial) {
    mdSendCommand (6, 8, serial);
}
void MdSerialComBinary::mdSendCommand (quint8 cmd, quint8 subcmd, quint8 serial) {
    QByteArray t;
    t.push_back ( (quint8) cmd );
    t.push_back ( (quint8) subcmd );
    t.push_back ( (quint8) serial );
    qDebug() << "mdSendCommand tx length=" << t.length() << " data=" << t.toHex();
    transmitMsg(t);
}

void MdSerialComBinary::mdCmdSetSerialFrequency (quint16 frequency, quint8 serial) {
    quint16 s = 1000 / frequency;
    QByteArray t;
    t.push_back ( (quint8) 6 );
    t.push_back ( (quint8) 11 );
    t.push_back ( (quint8) serial );
    t.push_back ( (quint8) (s & 0xFF) );
    t.push_back ( (quint8) (s >> 8) );
    qDebug() << "mdCmdSetSerialFrequency frequency=" << frequency << " hz (" << s << ") " << t.toHex();
    transmitMsg(t);
}

void MdSerialComBinary::mdCmdReadGearbox (quint8 serial) {
    qDebug() << "MdSerialComBinary::mdCmdReadGearbox serial=" << serial;
    QByteArray t;
    t.push_back ( (quint8) 6 );
    t.push_back ( (quint8) 13 );
    t.push_back ( (quint8) serial );
    transmitMsg(t);
}

void MdSerialComBinary::mdCmdWriteGearbox (double g1, double g2, double g3, double g4, double g5, double g6, quint8 serial) {
    qDebug() << "MdSerialComBinary::mdCmdWriteGearbox serial=" << serial;
    QByteArray t;
    t.push_back ( (quint8) 6 );
    t.push_back ( (quint8) 14 );
    t.push_back ( (quint8) serial );
    t.push_back ( (quint8) 6 );
    t.push_back ( double2_fixed_b1000_Ba(g1) );
    t.push_back ( double2_fixed_b1000_Ba(g2) );
    t.push_back ( double2_fixed_b1000_Ba(g3) );
    t.push_back ( double2_fixed_b1000_Ba(g4) );
    t.push_back ( double2_fixed_b1000_Ba(g5) );
    t.push_back ( double2_fixed_b1000_Ba(g6) );
    transmitMsg(t);
    qDebug() << "tx=" << t.toHex();
}

void MdSerialComBinary::debugDataGenUpdate() {
    debugTime += 1000;
    debugRPMCounter += 100;
    if ( debugRPMCounter > 7500 )
        debugRPMCounter = 850;
    int thr = 0;
    double df_ignition_total_retard = 0;
    if ( debugRPMCounter > 3000 ) {
        thr = 1;
        df_ignition_total_retard=48;
    }

    MdSensorRecord *sr = new MdSensorRecord ();
    sr->setTime( debugTime );
    sr->setRpm(debugRPMCounter);
    sr->setThrottle(thr);
    sr->df_boost_raw = 128;
    sr->df_ignition_total_retard = df_ignition_total_retard;

    data->addDataRecord ( new MdDataRecord (sr), AppEngine::getInstance()->getActualizeVis1() );
    emit showStatusBarSampleCount ( QString::number (data->size()) );
}
