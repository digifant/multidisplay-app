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
#include <QObject>
#include <QString>

class Map16x1 : public QObject
{
public:
    Map16x1();
    virtual ~Map16x1();
    virtual double mapValue ( int dval );
    virtual double mapValue8Bit ( int dval );
    virtual double mapValue10Bit ( int dval );
    virtual double mapValue12Bit ( int dval );
    virtual void testIt ();
    virtual QString name () = 0;

protected:
    QVector<double> mapData;
};


class Map16x1_NTC_ECT : public Map16x1 {
public:
    Map16x1_NTC_ECT();

    virtual void testIt ();
    QString name () { return "DF_ECT"; };
};

class Map16x1_NTC_IAT : public Map16x1 {
public:
    Map16x1_NTC_IAT();

    virtual void testIt ();
    QString name () { return "DF_IAT"; };
};

class Map16x1_ISV : public Map16x1 {
public:
    Map16x1_ISV();
    QString name () { return "DF_ISV"; };
};

class Map16x1_Voltage : public Map16x1 {
public:
    Map16x1_Voltage();
    double mapValue(int dval);
    QString name () { return "DF_Voltage"; };
};

class Map16x1_NbLambda : public Map16x1 {
public:
    Map16x1_NbLambda();
    QString name () { return "DF_NbLambda"; };
    double mapValue ( int dval );
};

class Map16x1_RPM6500 : public Map16x1 {
public:
    Map16x1_RPM6500();
    QString name () { return "DF_RPM_6500"; };
};
class Map16x1_RPM7350 : public Map16x1 {
public:
    Map16x1_RPM7350();
    QString name () { return "DF_RPM_7350"; };
};

/**
 * @brief The Map16x1_10Bit_VDO5 class maps 10 Bit AD values to millibars of a vdo 5 bar sensor
 */
class Map16x1_10Bit_VDO5 : public Map16x1 {
public:
    Map16x1_10Bit_VDO5();
    virtual double mapValue ( int dval );
    virtual void testIt ();
    QString name () { return "VDO5"; };
};

/**
 * @brief The Map16x1_10Bit_VDO5 class maps 10 Bit AD values to millibars of a vdo 10 bar sensor
 */
class Map16x1_10Bit_VDO10 : public Map16x1 {
public:
    Map16x1_10Bit_VDO10();
    virtual double mapValue ( int dval );
    virtual void testIt ();
    QString name () { return "VDO10"; };
};


#endif // MAP16X1_H
