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


#include "WotEventsDialog.h"
#include "ui_WotEventsDialog.h"
#include <QDebug>

#include "AppEngine.h"
#include "MdData.h"

WotEventsDialog::WotEventsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WotEventsDialog)
{
    ui->setupUi(this);

    connect (ui->buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    connect (ui->jumpButton, SIGNAL(clicked()), this, SLOT(jumpTo()));
    connect (ui->tableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(cellDoubleClicked(int,int)));
}

WotEventsDialog::~WotEventsDialog()
{
    delete ui;
}

void WotEventsDialog::jumpTo() {
    QList<QTableWidgetItem *> sel = ui->tableWidget->selectedItems ();
    int i = -1;

    foreach ( QTableWidgetItem *item , sel ) {
        //i = ui->tableWidget->itemAt (item->row(), 0 )->data(Qt::DisplayRole).toInt() ;
        i = idxList.at(item->row());
        qDebug() << "WotEventsDialog::jumpTo() row= " << item->row() << " idx_to_show=" << i;
    }
    if ( i > -1 )
        emit jumpToDataIdx(i);
}

void WotEventsDialog::show ( QList<int> idxL ) {
    idxList = idxL;
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setRowCount( idxL.size() );

    QList<QString> tl;
    tl << "time" << "RPM" << "boost";
    ui->tableWidget->setHorizontalHeaderLabels( tl );


    int row = 0;
    foreach ( int i , idxL ) {       
        QTime t = QTime(0, 0, 0, 0);
        t = t.addMSecs( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getTime() );
        QTableWidgetItem *wi = new QTableWidgetItem( t.toString("hh:mm:ss.zzz") );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,0,wi);


        wi = new QTableWidgetItem( QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getRpm() ) );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,1,wi);

        wi = new QTableWidgetItem( QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getBoost() ) );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,2,wi);

        row++;
    }

    QDialog::show();
}

void WotEventsDialog::show ( QList<QPair<int,int>> idxPL ) {
    QList<int> idxL;
    foreach ( auto i , idxPL ) {
        idxL.append ( i.first );
    }

    idxList = idxL;
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(7);
    ui->tableWidget->setRowCount( idxL.size() );

    QList<QString> tl;
    tl << "time" << "RPM min" << "RPM max" << "gear" << "min boost" << "max boost" << "mean boost";
    ui->tableWidget->setHorizontalHeaderLabels( tl );
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    int row = 0;
    QMap<int,int> gearM;
    foreach ( int i , idxL ) {
        double boost_max = 0;
        double boost_min = 7;
        double boost_mean = 0;
        int rpm_min = 12000;
        int rpm_max = 0;

        QString gearS("no gear data");
        uint p = 0;
        for ( int i = idxPL.at(row).first ; i <= idxPL.at(row).second ; i++ ) {
            double boost = AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getBoost();
            int gear = AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getGear();
            int rpm = AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getRpm();
            gearM[gear] = 1;
            boost_mean += boost;
            if ( boost > boost_max )
                boost_max = boost;
            if ( boost > 0 && boost < boost_min)
                boost_min = boost;
            if ( rpm > rpm_max )
                rpm_max = rpm;
            if ( rpm < rpm_min )
                rpm_min = rpm;
            ++p;
        }
        boost_mean = boost_mean / p;
        if ( ! gearM.empty()) {
            gearS = "";
            foreach ( int i, gearM.keys() ) {
                gearS = gearS + QString::number(i) + " ";
            }
        }

        QTime t = QTime(0, 0, 0, 0);
        t = t.addMSecs( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getTime() );

        QTableWidgetItem *wi = new QTableWidgetItem( t.toString("hh:mm:ss.zzz") );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,0,wi);


        wi = new QTableWidgetItem( QString::number( rpm_min ) );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,1,wi);

        wi = new QTableWidgetItem( QString::number( rpm_max ) );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,2,wi);

        wi = new QTableWidgetItem( gearS );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,3,wi);

        wi = new QTableWidgetItem( QString::number( boost_min ) );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,4,wi);

        wi = new QTableWidgetItem( QString::number( boost_max ) );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,5,wi);

        wi = new QTableWidgetItem( QString::number( boost_mean ) );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,6,wi);

        row++;
    }

    QDialog::show();
}


void WotEventsDialog::showKnock ( QList<int> idxL ) {
    idxList = idxL;
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setRowCount( idxL.size() );

    QList<QString> tl;
    tl << "time" << "RPM" << "boost" << "max knock retard";
    ui->tableWidget->setHorizontalHeaderLabels( tl );

    int row = 0;
    foreach ( int i , idxL ) {
        QTime t = QTime(0, 0, 0, 0);
        t = t.addMSecs( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getTime() );

        QTableWidgetItem *wi = new QTableWidgetItem( t.toString("hh:mm:ss.zzz") );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,0,wi);

        wi = new QTableWidgetItem( QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getRpm() ) );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,1,wi);

        wi = new QTableWidgetItem( QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getBoost() ) );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,2,wi);

        wi = new QTableWidgetItem( QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->df_ignition_total_retard ) );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,3,wi);

        row++;
    }

    QDialog::show();
}

void WotEventsDialog::showEGT ( QList<int> idxL ) {
    idxList = idxL;
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setRowCount( idxL.size() );

    QList<QString> tl;
    tl << "time" << "RPM" << "boost" << "EGT";
    ui->tableWidget->setHorizontalHeaderLabels( tl );

    int row = 0;
    foreach ( int i , idxL ) {
        QTime t = QTime(0, 0, 0, 0);
        t = t.addMSecs( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getTime() );
        QTableWidgetItem *wi = new QTableWidgetItem( t.toString("hh:mm:ss.zzz") );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,0,wi);

        wi = new QTableWidgetItem( QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getRpm() ) );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,1,wi);

        wi = new QTableWidgetItem( QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getBoost() ) );
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,2,wi);

        wi = new QTableWidgetItem( QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getHighestEgt()["temp"] )
                                   + " " + QChar(0x00B0) + "C (" + QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getHighestEgt()["idx"]) + ")");
        wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget->setItem (row,3,wi);

        row++;
    }

    QDialog::show();
}


void WotEventsDialog::show ( QMap<QString,QMap<QString,QVariant > > &d ) {
    QList<QString> tl;
    QList<int> newIdxList;

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(6);
    tl << "event" << "time" << "RPM" << "boost" << "EGT" << "knock";
    ui->tableWidget->setHorizontalHeaderLabels( tl );

    int eRow=0;
    foreach (QString e, d.keys()) {
        eRow += d[e]["data"].toList().size();
    }
    ui->tableWidget->setRowCount( eRow );

    int row = 0;
    foreach (QString e, d.keys()) {
        //events like EGT, knock
        QList<QVariant> dataL = d[e]["data"].toList();

        foreach ( QVariant v, dataL ) {
            int i = v.toInt();
            newIdxList.append(i);

            QTableWidgetItem  *wi = new QTableWidgetItem( e );
            if ( e == "Inj duty cycle") {
                wi->setText( wi->text() + " " +
                             QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->df_inj_duty,
                                              'f', 1) + " %");
            }
            wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            ui->tableWidget->setItem (row,0,wi);

            if ( d[e].contains("icon") ) {
                //z.B. "dialog-information"
                wi->setIcon( QIcon::fromTheme(d[e]["icon"].toString()) );
            }
            if ( e == "Inj duty cycle") {
                if ( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->df_inj_duty >= 100 )
                    wi->setIcon( QIcon::fromTheme("dialog-error") );
            }
            if ( e == "EGT") {
                if ( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->df_inj_duty >= 980 )
                    wi->setIcon( QIcon::fromTheme("dialog-error") );
            }

            QTime t = QTime(0, 0, 0, 0);
            t = t.addMSecs( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getTime() );
            wi = new QTableWidgetItem( t.toString("hh:mm:ss.zzz") );
            wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            ui->tableWidget->setItem (row,1,wi);

            wi = new QTableWidgetItem( QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getRpm() ) );
            wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            ui->tableWidget->setItem (row,2,wi);

            wi = new QTableWidgetItem( QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getBoost() ) );
            wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            ui->tableWidget->setItem (row,3,wi);

            wi = new QTableWidgetItem( QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getHighestEgt()["temp"] )
                                       + " " + QChar(0x00B0) + "C (" + QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->getHighestEgt()["idx"]) + ")");
            ui->tableWidget->setItem (row,4,wi);

            wi = new QTableWidgetItem( QString::number( AppEngine::getInstance()->getData()->getData()[i]->getSensorR()->df_ignition_total_retard ) );
            wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            ui->tableWidget->setItem (row,5,wi);

            row++;
        }
    }

    ui->tableWidget->resizeColumnsToContents();

    idxList = newIdxList;

    QDialog::show();
}


void WotEventsDialog::cellDoubleClicked ( int row, int column ) {
    qDebug() << "WotEventsDialog::cellDoubleClicked " << row << " " << column;
//    emit jumpToDataIdx( ui->tableWidget->itemAt (row, 0 )->data(Qt::DisplayRole).toInt() );
    emit jumpToDataIdx( idxList.at (row) );
}

