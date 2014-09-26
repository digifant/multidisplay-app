/*
    Copyright 2011 Dominik Gummel

    This file is part of MultidisplayUI.

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

#include "Map16x1.h"

#include <QDebug>

Map16x1::Map16x1() {
}


double Map16x1::mapValue ( int dval ) {
    int idx = dval >> 4;
    int w = dval & 0xF;
    return idx < 0xF ? (( ( w * mapData[idx+1] ) + ( -1 * (w-16) * mapData[idx]) ) / 16) : mapData[idx];
}

void Map16x1::testIt () {
    qDebug() << "implement me!";
}



Map16x1_NTC_ECT::Map16x1_NTC_ECT() {
    mapData.resize(17);
    /*
     -20
     -10
     0  102
     10  80
     20 60 (2,5K)
     30 50
     40 30 (1,25K)
     50  22,5
     60 15
     70 12
     80 9
     90  7
     100 5
     110 3
     120 2
     */
    mapData[0] = 130; //0
    mapData[1] = 65; //16
    mapData[2] = 38; //32
    mapData[3] = 28; //48
    mapData[4] = 18; //64
    mapData[5] = 10; //80
    mapData[6] = 2; //96
    mapData[7] = -6;
    mapData[8] = -14;
    mapData[9] = -22;
    mapData[10] = -30;
    mapData[11] = -38;
    mapData[12] = -46;
    mapData[13] = -54;
    mapData[14] = -62;
    mapData[15] = -62;
    mapData[16] = -62;
}


void Map16x1_NTC_ECT::testIt () {
    QList<int> v;
    v.append(200);
    v.append(180);
    v.append(150);
    v.append(130);
    v.append(102);
    v.append(80);
    v.append(70);
    v.append(60);
    v.append(50);
    v.append(44);
    v.append(30);
    v.append(28);
    v.append(22);
    v.append(15);
    v.append(12);
    v.append(9);
    v.append(7);
    v.append(5);
    v.append(3);
    v.append(2);

    foreach ( int i,  v ) {
        qDebug() << "df val=" << i << " mapped value=" << mapValue(i) << endl;
    }
}



Map16x1_NTC_IAT::Map16x1_NTC_IAT() {
    mapData.resize(17);
    /*
     -20
     -10
     0  102
     10  80
     20 60 (2,5K)
     30 50
     40 30 (1,25K)
     50  22,5
     60 15
     70 12
     80 9
     90  7
     100 5
     110 3
     120 2
     */
    mapData[0] = 117; //0
    mapData[1] = 108; //16
    mapData[2] = 99; //32
    mapData[3] = 90; //48
    mapData[4] = 81; //64
    mapData[5] = 72; //80
    mapData[6] = 63; //96
    mapData[7] = 54;
    mapData[8] = 45;
    mapData[9] = 36;
    mapData[10] = 27;
    mapData[11] = 18;
    mapData[12] = 9;
    mapData[13] = 0;
    mapData[14] = -9;
    mapData[15] = -18;
    mapData[16] = -27;
}


void Map16x1_NTC_IAT::testIt () {
    QList<int> v;
    v.append(250);
    v.append(240);
    v.append(220);
    v.append(200);
    v.append(180);
    v.append(150);
    v.append(130);
    v.append(102);
    v.append(80);
    v.append(70);
    v.append(60);
    v.append(50);
    v.append(44);
    v.append(30);
    v.append(28);
    v.append(22);
    v.append(15);
    v.append(12);
    v.append(9);
    v.append(7);
    v.append(5);
    v.append(3);
    v.append(2);

    foreach ( int i,  v ) {
        qDebug() << "df val=" << i << " mapped value=" << mapValue(i) << endl;
    }
}


Map16x1_ISV::Map16x1_ISV() {
    mapData.resize(17);
    mapData[0]=0;
    mapData[1]=0x23;
    mapData[2]=0x42;
    mapData[3]=0x60;
    mapData[4]=0x7C;
    mapData[5]=0x93;
    mapData[6]=0xA7;
    mapData[7]=0xB7;
    mapData[8]=0xC6;
    mapData[9]=0xD1;
    mapData[10]=0xDB;
    mapData[11]=0xE4;
    mapData[12]=0xEB;
    mapData[13]=0xF1;
    mapData[14]=0xF7;
    mapData[15]=0xFB;
    mapData[16]=0xFB;
}


Map16x1_Voltage::Map16x1_Voltage() {
    mapData.resize(17);
    mapData[0]=0;
    mapData[1]=1.6;
    mapData[2]=3.2;
    mapData[3]=4.8;
    mapData[4]=6.4;
    mapData[5]=8;
    mapData[6]=9.6;
    mapData[7]=11.2;
    mapData[8]=12.8;
    mapData[9]=14.3;
    mapData[10]=15.9;
    mapData[11]=17.5;
    mapData[12]=19.1;
    mapData[13]=20.7;
    mapData[14]=22.3;
    mapData[15]=23.9;
    mapData[16]=25.5;
}


Map16x1_RPM6500::Map16x1_RPM6500() {
    mapData.resize(17);
    mapData[0]=6500;
    mapData[1]=600;
    mapData[2]=5501;
    mapData[3]=5000;
    mapData[4]=4500;
    mapData[5]=4000;
    mapData[6]=3500;
    mapData[7]=3200;
    mapData[8]=2900;
    mapData[9]=2600;
    mapData[10]=2300;
    mapData[11]=1900;
    mapData[12]=1500;
    mapData[13]=1200;
    mapData[14]=900;
    mapData[15]=700;
    mapData[16]=500;
}

Map16x1_RPM7350::Map16x1_RPM7350() {
    mapData.resize(17);
    mapData[0]=7350;
    mapData[1]=6850;
    mapData[2]=6350;
    mapData[3]=5850;
    mapData[4]=5350;
    mapData[5]=4850;
    mapData[6]=4350;
    mapData[7]=3900;
    mapData[8]=3450;
    mapData[9]=3050;
    mapData[10]=2600;
    mapData[11]=2150;
    mapData[12]=1700;
    mapData[13]=1300;
    mapData[14]=1000;
    mapData[15]=700;
    mapData[16]=500;
}
