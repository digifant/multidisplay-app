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

Map16x1::~Map16x1() {

}

double Map16x1::mapValue ( int dval ) {
    return mapValue8Bit ( dval );
}

double Map16x1::mapValue8Bit ( int dval ) {
    if ( dval > 255 )
        dval = 255;
    int idx = dval >> 4;
    int w = dval & 0xF;
    return idx < 0xF ? (( ( w * mapData[idx+1] ) + ( -1 * (w-16) * mapData[idx]) ) / 16) : mapData[idx];
}
double Map16x1::mapValue10Bit ( int dval ) {
    if ( dval > 1023 )
        dval = 1023;
    int idx = dval >> 6;
    int w = dval - 64*idx;
    return idx < 0xF ? (( ( w * mapData[idx+1] ) + ( -1 * (w-64) * mapData[idx]) ) / 64) : mapData[idx];
}
double Map16x1::mapValue12Bit ( int dval ) {
    if ( dval > 4095 )
        dval = 4095;
    int idx = dval >> 8;
    int w = dval - 256*idx;
    return idx < 0xF ? (( ( w * mapData[idx+1] ) + ( -1 * (w-256) * mapData[idx]) ) / 256) : mapData[idx];
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
        qDebug() << "df val=" << i << " mapped value=" << mapValue(i) << Qt::endl;
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
        qDebug() << "df val=" << i << " mapped value=" << mapValue(i) << Qt::endl;
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
    mapData[0]=0.00;
    mapData[1]=1.49;
    mapData[2]=2.98;
    mapData[3]=4.48;
    mapData[4]=5.97;
    mapData[5]=7.46;
    mapData[6]=8.95;
    mapData[7]=10.44;
    mapData[8]=11.93;
    mapData[9]=13.43;
    mapData[10]=14.92;
    mapData[11]=16.41;
    mapData[12]=17.90;
    mapData[13]=19.39;
    mapData[14]=20.89;
    mapData[15]=22.38;
    mapData[16]=23.78;
}
double Map16x1_Voltage::mapValue(int dval)
{
    return dval * 1127/12087.0;
}


Map16x1_NbLambda::Map16x1_NbLambda() {
    //https://bimmerprofs.com/wp-content/uploads/2017/07/o2sensor_output_afetr_CO_converter.png
    //map 0-1V to 0-255
    mapData.resize(17);
    mapData[0]=1.2;
    mapData[1]=1.1;
    mapData[2]=1.01;
    mapData[3]=1.0012;
    mapData[4]=1.0009;
    mapData[5]=1.0006;
    mapData[6]=1.0003;
    mapData[7]=1;
    mapData[8]=1;
    mapData[9]=0.9997;
    mapData[10]=0.9994;
    mapData[11]=0.9991;
    mapData[12]=0.995;
    mapData[13]=0.99;
    mapData[14]=0.9;
    mapData[15]=0.8;
    mapData[16]=0.69;
}
double Map16x1_NbLambda::mapValue ( int dval ) {
    int m255 = qRound (dval / 3.9216);
    return Map16x1::mapValue( m255 );
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


Map16x1_10Bit_VDO5::Map16x1_10Bit_VDO5() {
    mapData.resize(17);
    mapData[0]=0;
    mapData[1]=110;
    mapData[2]=573;
    mapData[3]=1113;
    mapData[4]=1751;
    mapData[5]=2515;
    mapData[6]=2958;
    mapData[7]=3999;
    mapData[8]=5319;
    mapData[9]=5319;
    mapData[10]=5319;
    mapData[11]=5319;
    mapData[12]=5319;
    mapData[13]=5319;
    mapData[14]=5319;
    mapData[15]=5319;
    mapData[16]=5319;
}
double Map16x1_10Bit_VDO5::mapValue ( int dval ) {
    return mapValue10Bit( dval );
}
void Map16x1_10Bit_VDO5::testIt () {
    QList<int> v;
    v.append(0);
    v.append(64);
    v.append(96);
    v.append(749);
    v.append(888);
    v.append(928);
    v.append(960);
    v.append(1024);

    foreach ( int i,  v ) {
        qDebug() << "ad value=" << i << " mapped value=" << mapValue(i) << Qt::endl;
    }
}


Map16x1_10Bit_VDO10::Map16x1_10Bit_VDO10() {
    mapData.resize(17);
    mapData[0]=0;
    mapData[1]=207;
    mapData[2]=1006;
    mapData[3]=1962;
    mapData[4]=3126;
    mapData[5]=4583;
    mapData[6]=6474;
    mapData[7]=9067;
    mapData[8]=10796;
    mapData[9]=10796;
    mapData[10]=10796;
    mapData[11]=10796;
    mapData[12]=10796;
    mapData[13]=10796;
    mapData[14]=10796;
    mapData[15]=10796;
    mapData[16]=10796;
}
double Map16x1_10Bit_VDO10::mapValue ( int dval ) {
    return mapValue10Bit( dval );
}
void Map16x1_10Bit_VDO10::testIt () {
    QList<int> v;
    v.append(0);
    v.append(64);
    v.append(96);
    v.append(448);
    v.append(480);
    v.append(512);
    v.append(749);
    v.append(888);

    foreach ( int i,  v ) {
        qDebug() << "ad value=" << i << " mapped value=" << mapValue(i) << Qt::endl;
    }
}
