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

#include "N75PidSettingsWidget.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QObject>
#include "com/MdBinaryProtocol.h"


N75PidSettingsTableWidget::N75PidSettingsTableWidget(QWidget *parent)
    : MyTableWidget (parent)
{

}

/* ========================================================================================  */



N75PidSettingsTableWidgetHorizontal::N75PidSettingsTableWidgetHorizontal (QWidget *parent)
    : N75PidSettingsTableWidget (parent) {
    setRowCount(2);
    setColumnCount(10);

    QList<QString> tl;
    tl << "a Kp" << "a Ki" << "a Kd" << "c Kp" << "c Ki" << "c Kd" << "a factor" << "c factor" << "enabled" << "bst limit" ;
    setHorizontalHeaderLabels( tl );
    tl.clear();
    tl << "current" << "new";
    setVerticalHeaderLabels( tl );

    for ( int i = 0 ; i < rowCount() ; i++ )
        setRowHeight(i, 20);

    tl.clear();
    tl << "aggressive Kp" << "aggressive Ki" << "aggressive Kd" << "conservative Kp" << "conservative Ki"
       << "conservative Kd" << "aggressive activation threshold factor"
       << "conservative threshold activation factor" << "PID enabled" << "max boost (hard limit)";
    for ( int i = 0 ; i < columnCount() ; i++ ) {
        setColumnWidth(i, 65);
        horizontalHeaderItem(i)->setToolTip(tl.at(i));
    }
    for ( int r = 0 ; r < rowCount() ; r++ ) {
        for ( int c = 0 ; c < columnCount() ; c++ ) {
            QTableWidgetItem *wi = new MyTableWidgetItem();
            Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
            if ( r==1 )
                f=Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
            wi->setFlags(f);
            setItem(r,c, wi);
        }
    }
    resizeColumnsToContents();
}

/* ========================================================================================  */


N75PidSettingsTableWidgetVertical::N75PidSettingsTableWidgetVertical (QWidget *parent)
    : N75PidSettingsTableWidget (parent) {
    setRowCount(10);
    setColumnCount(2);

    QList<QString> tl;
    tl << "a Kp" << "a Ki" << "a Kd" << "c Kp" << "c Ki" << "c Kd" << "a factor" << "c factor" << "enabled" << "bst limit" ;
    setVerticalHeaderLabels( tl );
    tl.clear();
    tl << "current" << "new";
    setHorizontalHeaderLabels( tl );

    for ( int i = 0 ; i < rowCount() ; i++ )
        setRowHeight(i, 20);

    tl.clear();
    tl << "aggressive Kp" << "aggressive Ki" << "aggressive Kd" << "conservative Kp" << "conservative Ki"
       << "conservative Kd" << "aggressive activation threshold factor"
       << "conservative threshold activation factor" << "PID enabled" << "max boost (hard limit)";
    for ( int i = 0 ; i < rowCount() ; i++ ) {
//        setColumnWidth(i, 65);
        verticalHeaderItem(i)->setToolTip(tl.at(i));
    }
    for ( int r = 0 ; r < columnCount() ; r++ ) {
        for ( int c = 0 ; c < rowCount() ; c++ ) {
            QTableWidgetItem *wi = new MyTableWidgetItem();
            Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
            if ( r==1 )
                f=Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
            wi->setFlags(f);
            setItem(r,c, wi);
        }
    }
    resizeColumnsToContents();
    resizeRowsToContents();
}

/* ========================================================================================  */

N75PidSettingsWidget::N75PidSettingsWidget (QWidget *parent, MdBinaryProtocol* mds, bool horizontal) : QGroupBox(parent), mds(mds) {
    setLayout( new QHBoxLayout (this) );
    if ( horizontal )
        tw = new N75PidSettingsTableWidgetHorizontal (this);
    else
        tw = new N75PidSettingsTableWidgetVertical (this);
    tw->setMinimumSize(200,60);
//    tw->setFixedHeight(120);
    tw->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    layout()->addWidget( tw );
    setTitle("PID Parameter");

    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

    bFrame = new QFrame (this);
    layout()->addWidget( bFrame );
    bFrame->setLayout( new QVBoxLayout(this) );
    bFrame->setMinimumSize(60, 60);
    bFrame->setMinimumSize(120, 120);
//    bBox->setFixedHeight(120);
//    bBox->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
    bFrame->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    bFrame->setContentsMargins(0,0,0,0);

    bRead = new QPushButton (bFrame);
    bRead->setText("read");
    bRead->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    bWrite = new QPushButton (bFrame);
    bWrite->setText("write");
    bWrite->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    bReadEeprom = new QPushButton (bFrame);
    bReadEeprom->setText("EE read");
    bReadEeprom->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    bWriteEeprom = new QPushButton (bFrame);
    bWriteEeprom->setText("EE write");
    bWriteEeprom->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

    bFrame->layout()->addWidget(bRead);
    bFrame->layout()->addWidget(bWrite);
    bFrame->layout()->addWidget(bReadEeprom);
    bFrame->layout()->addWidget(bWriteEeprom);

    connect (bWrite, SIGNAL(clicked()), this, SLOT(writeIt()));
    connect (bRead, SIGNAL(clicked()), this, SLOT(readSettings()));
    connect (bWriteEeprom, SIGNAL(clicked()), this, SLOT(save2Eeprom()));
    connect (bReadEeprom, SIGNAL(clicked()), this, SLOT(readFromEeprom()));


    if (mds) {
        connect (this, SIGNAL(requestN75PidSettings()), (QObject*)mds, SLOT(mdCmdReqN75Settings()));
        connect ( (QObject*) mds, SIGNAL(n75SettingsReceived(quint8,double,double,double,double,double,double,double, double, bool, double)),
                 this, SLOT(n75PidSettings(quint8, double,double,double,double,double,double,double,double,bool,double)));
        connect (this, SIGNAL(setN75PidSettings(quint8,double,double,double,double,double,double,double,double,bool,double)),
                 mds, SLOT(mdCmdWriteN75Settings(quint8,double,double,double,double,double,double,double,double,bool,double)));
        connect (this, SIGNAL(writeN75PidSettingsToEeprom()), mds, SLOT(mdCmdWriteN75SettingsToEEprom()));
        connect (this, SIGNAL(readN75PidSettingsFromEeprom()), mds, SLOT(mdCmdReadN75SettingsFromEEprom()));
    }
}

void N75PidSettingsWidget::n75PidSettings (quint8 serial,
                                           double aKp, double aKi, double aKd,
                                           double cKp, double cKi, double cKd,
                                           double aAT, double cAT, bool enabled, double maxBoost ) {
    tw->item(0,0)->setText (QString::number(aKp));
    tw->item(0,1)->setText (QString::number(aKi));
    tw->item(0,2)->setText (QString::number(aKd));
    tw->item(0,3)->setText (QString::number(cKp));
    tw->item(0,4)->setText (QString::number(cKi));
    tw->item(0,5)->setText (QString::number(cKd));
    tw->item(0,6)->setText (QString::number(aAT));
    tw->item(0,7)->setText (QString::number(cAT));
    tw->item(0,8)->setText (QString::number(enabled));
    tw->item(0,9)->setText (QString::number(maxBoost));

    qDebug() << "N75PidSettingsWidget::n75PidSettings aKp=" << aKp << " aKi=" << aKi << " aKd" << aKd
             << " cKp" << cKp << " cKi=" << cKi << " cKp=" << cKp << " aAT=" << aAT << " cAT=" << cAT
                << " enabled=" << enabled << " boost limit=" << maxBoost;
}

void N75PidSettingsWidget::writeIt () {
    if ( ! tw->item(1,0)->data(Qt::DisplayRole).isValid() ) {
        qDebug() << "aKp not valid -> write abort";
        return;
    }
    double aKp = tw->item(1,0)->data(Qt::DisplayRole).toDouble();
    double aKi = tw->item(1,1)->data(Qt::DisplayRole).toDouble();
    double aKd = tw->item(1,2)->data(Qt::DisplayRole).toDouble();
    double cKp = tw->item(1,3)->data(Qt::DisplayRole).toDouble();
    double cKi = tw->item(1,4)->data(Qt::DisplayRole).toDouble();
    double cKd = tw->item(1,5)->data(Qt::DisplayRole).toDouble();
    double aAT = tw->item(1,6)->data(Qt::DisplayRole).toDouble();
    double cAT = tw->item(1,7)->data(Qt::DisplayRole).toDouble();
    double enableD = tw->item(1,8)->data(Qt::DisplayRole).toDouble();
    bool enable = false;
    if (enableD == 1)
        enable = true;
    double maxBoost = tw->item(1,9)->data(Qt::DisplayRole).toDouble();
    qDebug() << "N75PidSettingsWidget::writeIt aKp=" << aKp << " aKi=" << aKi << " aKd=" << aKd
           << " cKp=" << cKp << " cKi=" << cKi << " cKd=" << cKd
           << " aAT=" << aAT << " cAT=" << cAT << " enable=" << enable << " boost limit=" << maxBoost;
    emit setN75PidSettings (0, aKp, aKi, aKd, cKp, cKi, cKd, aAT, cAT, enable, maxBoost);
}



