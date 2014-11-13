/*
    Copyright 2012 Dominik Gummel

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

#ifndef N75PIDSETTINGSWIDGET_H
#define N75PIDSETTINGSWIDGET_H

#include <QObject>
#include <QGroupBox>
#include "MyTableWidget.h"

class MdBinaryProtocol;

class N75PidSettingsTableWidget : public MyTableWidget {
    Q_OBJECT
public:
    N75PidSettingsTableWidget (QWidget *parent = 0);
signals:

public slots:

protected:
};

class N75PidSettingsWidget : public QGroupBox {
    Q_OBJECT
public:
    N75PidSettingsWidget (QWidget *parent = 0, MdBinaryProtocol* mds = 0 );
signals:
    void requestN75PidSettings();
    void setN75PidSettings (quint8 serial, double aKp, double aKi, double aKd,
                            double cKp, double cKi, double cKd,
                            double aAT, double cAT, bool enabled, double maxBoost );
    void readN75PidSettingsFromEeprom();
    void writeN75PidSettingsToEeprom();

public slots:
    void n75PidSettings (quint8 serial,
                         double aKp, double aKi, double aKd,
                         double cKp, double cKi, double cKd,
                         double aAT, double cAT, bool enabled, double maxBoost );
private slots:
    void writeIt ();
    void readFromEeprom() { emit readN75PidSettingsFromEeprom(); };
    void save2Eeprom() { emit writeN75PidSettingsToEeprom(); };
    void readSettings() { emit requestN75PidSettings(); };

protected:


    N75PidSettingsTableWidget* tw;
    QFrame *bFrame;
//    QFrame *bFrame;
    QPushButton *bRead;
    QPushButton *bWrite;
    QPushButton *bReadEeprom;
    QPushButton *bWriteEeprom;
    MdBinaryProtocol *mds;
};


#endif
