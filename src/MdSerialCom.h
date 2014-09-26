/*
    Copyright 2010 Stephan Martin, Dominik Gummel

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

#ifndef MDSERIALCOM_H_
#define MDSERIALCOM_H_


#include <QObject>
#include <QList>
#include <QTableWidgetItem>

class QextSerialPort;
class MdData;
class MdBoostPidRecord;
class MdSensorRecord;
class QTableWidget;

class QTimer;



class SerialData {
public:
    SerialData ();

    int getFixed32 (int n);

    float floats[6];
    int fixedfac;
};


/**
  *@deprecated
  MdSerialCom is DEPRECATED! Dont use it!
  Use MdSerialComBinary !
  */
class MdSerialCom : public QObject {
    Q_OBJECT
public:
    MdSerialCom(MdData* data);
    virtual ~MdSerialCom();

    bool changePortSettings (QString sport="/dev/ttyUSB0", QString speed="57600");


signals:
    void bytesRead (QByteArray);
    void stringRead (QString);
    void lineRead (QString);
    void showStatusMessage (QString);
    void showStatusBarSampleCount (QString);
    void newBoostPidMsg (MdBoostPidRecord*);

    void portOpened();
    void portClosed();

protected:

    bool setupPort (QString sport="/dev/ttyUSB0", QString speed="57600");

    QextSerialPort *port;
    QString ts;
    QByteArray ap, ah, bp, bh, mdso, mdcalboost, mdsave, mdload;
    QString buffer;
    MdData *data;
    MdBoostPidRecord *cbr;
    MdSensorRecord *csr;
    QTableWidget *boostTable;

    union {
        uchar asBytes[24];
        float asFloat[6];
    } serialPidData;

    int debugRPMCounter;
    int debugTime;
    QTimer *debugDataGenTimer;

    void parseLine (QString l);

public slots:
    //	void appendCR();
    //	void appendLF();
    void closePort();
    void openPort();

    void mdCmdAp();
    void mdCmdAh();
    void mdCmdBp();
    void mdCmdBh();

    void mdCmdActivateSerialOutput();
    void mdCmdActivateSerialBinaryOutput();
    void mdCmdDisableSerialOutput();
    void mdCmdCalBoost();
    void mdCmdReadEeprom();
    void mdCmdLoadFromEeprom();
    void mdCmdSave2Eeprom();
    void mdCmdN75ManualDuty (int normal, int race);

    void mdCmdBoostPidParam (double setpoint, int output, double p, double i, double d, bool autoMode);


protected slots:
    virtual void onReadyRead();
    void onDsrChanged(bool status);

    void transmitMsg(QString s);
    void transmitMsg(QByteArray ba);

    void receiveMsg();

    virtual void debugDataGenUpdate();

};

#endif /* MDSERIALCOM_H_ */
