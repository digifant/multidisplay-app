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


#ifndef MYTABLEWIDGET_H
#define MYTABLEWIDGET_H

#include <QTableWidget>
#include <QModelIndexList>
#include <QTableWidgetItem>
#include <QGroupBox>
#include <QPushButton>

#include <ColorOverBlend.h>

class MyTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit MyTableWidget(QWidget *parent = 0);

signals:

public slots:

protected:
    virtual void copy ();
    virtual void paste();

    virtual void keyPressEvent(QKeyEvent *event);
    QModelIndexList copyIndexList;
};

/* ************************* */

class N75TableWidget : public MyTableWidget
{
    Q_OBJECT
public:
    explicit N75TableWidget(quint8 mode, QWidget *parent = 0);
    ~N75TableWidget ();

signals:

public slots:
    void cellChanged(int,int);

    protected:
    virtual void copy ();
    virtual void paste();

    //! high(=1) or low(=0) boost maps
    quint8 mode;
    ColorOverBlend *dutyOb;
    ColorOverBlend *setpointOb;
};


/* ************************* */

class MyTableWidgetItem : public QTableWidgetItem {
public:
    MyTableWidgetItem ();
    virtual void setData ( int role, const QVariant & value );

private:

};

#endif // MYTABLEWIDGET_H
