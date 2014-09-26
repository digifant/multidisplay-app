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

#ifndef MAP16X1_H
#define MAP16X1_H

#include <QVector>

class Map16x1
{
public:
    Map16x1();
    double mapValue ( int dval );
    virtual void testIt ();

protected:
    QVector<double> mapData;
};


class Map16x1_NTC_ECT : public Map16x1 {
public:
    Map16x1_NTC_ECT();

    virtual void testIt ();
};

class Map16x1_NTC_IAT : public Map16x1 {
public:
    Map16x1_NTC_IAT();

    virtual void testIt ();
};

class Map16x1_ISV : public Map16x1 {
public:
    Map16x1_ISV();
};

class Map16x1_Voltage : public Map16x1 {
public:
    Map16x1_Voltage();
};

class Map16x1_RPM6500 : public Map16x1 {
public:
    Map16x1_RPM6500();
};
class Map16x1_RPM7350 : public Map16x1 {
public:
    Map16x1_RPM7350();
};


#endif // MAP16X1_H
