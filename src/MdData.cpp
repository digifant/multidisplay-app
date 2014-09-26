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


#include "MdData.h"
#include "BoostPlot.h"
#include "VisualizationPlot.h"
#include "MdDataRecordV1.h"
#include "MdDataRecordV2.h"
#include "widgets/realtimevis.h"
#include "AppEngine.h"
#include "TransferFunction.h"
#include "Map16x1.h"
#include "V2PowerDialog.h"
#include "WotEventsDialog.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <QFile>
#include <QTemporaryFile>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QtCore/qmath.h>
#include <math.h>

#if  defined (Q_WS_MAEMO_5)  || defined (ANDROID)
#include "mobile/Accelerometer.h"
#include "mobile/MobileGPS.h"
#endif

MaxDataSet::MaxDataSet () : p(NULL) {

}

MdData::MdData (QMainWindow* mw_boost, QWidget* parent_boost, QMainWindow* mw_vis1, QWidget* parent_vis1,
                QTableView* dataView)

{
    this->dataView = dataView;
    boostPidPlot = new BoostPidPlot ( mw_boost, parent_boost );
    boostPidPlot->replot();

//    visTabSplitter = new QSplitter(parent_vis1);
//    visTabSplitter->setOrientation(Qt::Vertical);

    visPlot = new VisualizationPlot ( mw_vis1, parent_vis1, dataView );

//    hbl = new QHBoxLayout();
//    hbl->addWidget(visTabSplitter);
//    parent_vis1->setLayout(hbl);

//    visTabSplitter->addWidget(visPlot);
//    visTabSplitter->addWidget(visPlot2);
//    qDebug() << visTabSplitter->sizes();
//    sl = new QList<int> ();
//    sl->append(500);
//    sl->append(200);
//    visTabSplitter->setSizes(*sl);
//    qDebug() << visTabSplitter->sizes();
    visPlot->replot();

    plotList.push_back ( (MdPlot*) boostPidPlot );
    plotList.push_back ( (MdPlot*) visPlot );

    headerColNames.push_back("Time");
    headerColNames.push_back("RPM");
    headerColNames.push_back("Boost");
    headerColNames.push_back("Throttle");
    headerColNames.push_back("Lambda");
    headerColNames.push_back("LMM");
    headerColNames.push_back("CaseTemp");
    headerColNames.push_back("AGT0");
    headerColNames.push_back("AGT1");
    headerColNames.push_back("AGT2");
    headerColNames.push_back("AGT3");
    headerColNames.push_back("AGT4");
    headerColNames.push_back("AGT5");
    headerColNames.push_back("AGT6");
    headerColNames.push_back("AGT7");
    headerColNames.push_back("Battery V");
    headerColNames.push_back("VDO Pres1");
    headerColNames.push_back("VDO Pres2");
    headerColNames.push_back("VDO Pres3");
    headerColNames.push_back("VDO Temp1");
    headerColNames.push_back("VDO Temp2");
    headerColNames.push_back("VDO Temp3");
    headerColNames.push_back("Speed");
    headerColNames.push_back("Gear");
    headerColNames.push_back("N75 duty");
    headerColNames.push_back("N75 Pid Bst");
    headerColNames.push_back("N75 Map duty");
    headerColNames.push_back("EFR speed");

    //Digifant
    headerColNames.push_back("DF Ignition");
    headerColNames.push_back("DF Ign Retard");
    headerColNames.push_back("DF Injection");
    headerColNames.push_back("DF IAT");
    headerColNames.push_back("DF ECT");
    headerColNames.push_back("DF raw knock");
    headerColNames.push_back("DF boost");
    headerColNames.push_back("DF cold startup enrich");
    headerColNames.push_back("DF warm startup enrich");
    headerColNames.push_back("DF ect enrich");
    headerColNames.push_back("DF iat enrich");
    headerColNames.push_back("DF counter startup enrich");
    headerColNames.push_back("DF voltage");
    headerColNames.push_back("DF acc enrich");
    headerColNames.push_back("DF ect inj addon");
    headerColNames.push_back("DF Lambda Raw");
    headerColNames.push_back("DF CO");
    headerColNames.push_back("DF ISV");
    headerColNames.push_back("DF LC flags");
    headerColNames.push_back("Kline dbg");

    headerColNames.push_back("GPS Coordinates");
    headerColNames.push_back("GPS Speed");
    headerColNames.push_back("GPs Altitude");
    headerColNames.push_back("Acceleration");

//    rtvis = 0;

    if ( dataView ) {
        dataView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect (dataView, SIGNAL(customContextMenuRequested( const QPoint& )),
                this, SLOT(tableDataView_customContextMenu( const QPoint& )));

        dataViewContextMenu = new QMenu (dataView);
        dataViewContextMenuSaveMarkedRows = dataViewContextMenu->addAction("save marked rows as new file");
        dataViewContextMenuDelItemAction = dataViewContextMenu->addAction("delete marked rows");
        dataViewContextMenuDigifantBoost2MdBoost = dataViewContextMenu->addAction("convert Digifant Boost to Md Boost");
        dataViewContextMenuShowinVis1 = dataViewContextMenu->addAction("show in Vis1");
        dataViewContextMenuPowerPlot = dataViewContextMenu->addAction("Power / Torque plot");
        dataViewContextMenuPowerPlotGPS = dataViewContextMenu->addAction("Power / Torque plot (GPS)");
        dataViewContextMenuCalc100to200 = dataViewContextMenu->addAction("Calc 100-200km/h time");
        dataViewContextMenuCalc100to200GPS = dataViewContextMenu->addAction("Calc 100-200km/h time (GPS)");
        dataViewContextMenuFindWotEvents = dataViewContextMenu->addAction("find WOT events");
        dataViewContextMenuFindKnockEvents = dataViewContextMenu->addAction("find Knock events");
        dataViewContextMenuFindHighEGTEvents = dataViewContextMenu->addAction("find high EGT (>950�C) events");
        dataViewContextMenuCheckEvents = dataViewContextMenu->addAction("check");
    }

    egtBlend = new ColorOverBlend (QColor( Qt::green), QColor(Qt::yellow), QColor(Qt::red), 880, 920, 940);
    efrBlend = new ColorOverBlend (QColor( Qt::green), QColor(Qt::yellow), QColor(Qt::red), 117000, 130000, 139000);
    knockBlend = new  ColorOverBlend (QColor( Qt::green), QColor(Qt::yellow), QColor(Qt::red), 0, 3, 6);
    boostBlend = new ColorOverBlend (QColor( Qt::green), QColor(Qt::yellow), QColor(Qt::red), 1.5, 2.0, 2.4);
    iatBlend = new ColorOverBlend (QColor( Qt::white), QColor(Qt::yellow), QColor(Qt::red), 35, 50, 65);
    voltageBlend = new ColorOverBlend (QColor( Qt::red), QColor(Qt::yellow), QColor(Qt::white), 12, 12.8, 13.4);
    lambdaBlend = new ColorOverBlend (QColor( Qt::green), QColor(Qt::yellow), QColor(Qt::red), 0.69, 1, 1.36);
    rawKnockBlend = new ColorOverBlend (QColor( Qt::green), QColor(Qt::yellow), QColor(Qt::red), 0, 60, 150);
    injectorDutyBlend = new ColorOverBlend (QColor( Qt::green), QColor(Qt::yellow), QColor(Qt::red), 0, 80, 90);

    dataTableConfigDialog = new DataTableConfigDialog();
    connect (dataTableConfigDialog, SIGNAL(accepted()), this, SLOT(acceptCfgDataTable()) );

    isvMap = new Map16x1_ISV();

    for ( uint8_t i = 0 ; i < MAXVALUES ; i++ )
        maxValues[i] = new MaxDataSet();

    powerDialog = new V2PowerDialog (mw_vis1);
    wotEventsDialog = new WotEventsDialog (mw_vis1);

    connect ( wotEventsDialog, SIGNAL(jumpToDataIdx(int)), this, SLOT(showDataListIdx(int)) );
}

MdData::~MdData() {
    foreach ( MdDataRecord* r , dataList )
        if (r)
            delete (r);
    dataList.clear();

    for ( uint8_t i = 0 ; i < MAXVALUES ; i++ )
        delete (maxValues[i]);

    if ( egtBlend )
        delete egtBlend;
    if (efrBlend )
        delete efrBlend;
    if ( boostBlend )
        delete boostBlend;
    if ( knockBlend )
        delete knockBlend;
    if ( lambdaBlend )
        delete lambdaBlend;
    if ( rawKnockBlend )
        delete rawKnockBlend;
    if ( injectorDutyBlend )
        delete injectorDutyBlend;

    if ( dataTableConfigDialog )
        delete dataTableConfigDialog;

    if (isvMap)
        delete isvMap;
}

void MdData::writeSettings () {
#if  !defined (Q_WS_MAEMO_5)  && !defined (ANDROID)
    QSettings settings("MultiDisplay", "UI");
    settings.beginGroup("DataTableView");
    for (int i=0 ; i<headerColNames.size() ; i++) {
        settings.setValue( QString::number(i), dataView->isColumnHidden(i) );
    }
    settings.endGroup();
#endif
}
void MdData::readSettings () {
#if  !defined (Q_WS_MAEMO_5)  && !defined (ANDROID)
    QSettings settings("MultiDisplay", "UI");
    settings.beginGroup("DataTableView");
    for (int i=0 ; i<headerColNames.size() ; i++) {
        dataView->setColumnHidden(i, settings.value( QString::number(i), QVariant (false) ).toBool() );
    }
    settings.endGroup();
#endif
}


QList<int> MdData::helperGetUniqueRows (QItemSelectionModel *select ) {
    QMap<int,int> res;
    QModelIndexList il = select->selectedIndexes();
    foreach (QModelIndex i, il) {
        res[i.row()]=1;
    }
    return res.keys();
}

void MdData::tableDataView_customContextMenu( const QPoint& pos) {
    qDebug() << "MdData::tableDataView_customContextMenu " << pos.x() << " " << pos.y();

    QItemSelectionModel *select = dataView->selectionModel();

    dataViewContextMenuDelItemAction->setEnabled( select->hasSelection() );
    dataViewContextMenuDigifantBoost2MdBoost->setEnabled( select->hasSelection() );

    QAction *a = dataViewContextMenu->exec(dataView->viewport()->mapToGlobal(pos));
    if (a == dataViewContextMenuDelItemAction) {
//        qDebug() << "MdData::tableDataView_customContextMenu del marked items";


        int size = dataList.size();
        //row has to be completely selected
        /*
        QModelIndexList sr = select->selectedRows(); // return selected row(s)
        QModelIndex i;
        foreach (i, sr) {
            removeRows( size - i.row(), 1);
        }
        */

        //only 1 cell of a row has to be selected
        QList<int> dr = helperGetUniqueRows ( select );
        foreach (int i, dr)
            removeRows ( size - i, 1);
    }

    if (a == dataViewContextMenuDigifantBoost2MdBoost) {
        TransferFunction *tf = AppEngine::getInstance()->getDfBoostTransferFunction();
        int size = dataList.size();
        //row has to be completely selected
        /*
        QModelIndexList sr = select->selectedRows(); // return selected row(s)
        QModelIndex i;
        foreach (i, sr) {
            MdSensorRecord *record = dataList.at( size - i.row() - 1)->getSensorR();
            //HACK set ambient pressure to 100kpa!
            record->setBoost( qFloor ( ((tf->map(record->df_boost_raw) - 100) / 100) *100) / 100.0 );
        }
        */
        //only 1 cell of a row has to be selected
        QList<int> dr = helperGetUniqueRows ( select );
        foreach (int i, dr) {
            MdSensorRecord *record = dataList.at( size - i - 1)->getSensorR();
            //HACK set ambient pressure to 100kpa!
            record->setBoost( qFloor ( ((tf->map(record->df_boost_raw) - 100) / 100) *100) / 100.0 );
        }
    }

    if ( a == dataViewContextMenuShowinVis1 ) {
        //first selected element
        Q_ASSERT (select != NULL);
        QList<int> rows;
        if ( select != NULL ) {
            //complete row is selected
            QModelIndexList sr = select->selectedRows();
            if ( ! sr.isEmpty() ) {
                //highest row index is the newest record set
                //we want to display the first (oldest) selected record!
                QModelIndex e = select->selectedRows().last();
                foreach (QModelIndex i, select->selectedRows() ) {
                    rows.append(i.row());
                }
            } else {
                //only 1 or more cells selected
                //row 0: newest record (top)
                //not sorted! we want the oldest one (highest row)
                QModelIndexList il = select->selectedIndexes();
                if ( ! il.isEmpty() ) {
                    foreach (QModelIndex i, il) {
                        rows.append(i.row());
                    }
                }
            }
            qSort (rows.begin(), rows.end());
            emit showRecordInVis1( rows.at ( rows.size()/2 ) );
            qDebug() << "show in vis1 " << rows.at ( rows.size()/2 )  << " time=" << dataList.at(dataList.size() - rows.at ( rows.size()/2 ) - 1)->getSensorR()->getTime();
        }
    }
    if ( a == dataViewContextMenuPowerPlot || a == dataViewContextMenuPowerPlotGPS ) {
        QList<int> rows;
        Q_ASSERT (select != NULL);
        if ( select != NULL ) {
            //complete row is selected
            QModelIndexList sr = select->selectedRows();
            if ( ! sr.isEmpty() ) {
                //row 0: newest record (top) = dataList.last() !
                foreach ( QModelIndex i , select->selectedRows() ) {
                    rows.append( dataList.size() - i.row() - 1 );
                }
            } else {
                //only 1 or more cells selected
                //row 0: newest record (top)
                QModelIndexList il = select->selectedIndexes();
                foreach (QModelIndex i, il) {
                    rows.append ( dataList.size() - i.row() - 1 );
                }
            }
            if ( ! rows.isEmpty() ) {
                qSort (rows.begin(), rows.end());
                if ( a == dataViewContextMenuPowerPlotGPS )
                    powerDialog->powerPlot()->setData (dataList, rows, true);
                else
                    powerDialog->powerPlot()->setData (dataList, rows, false);
                powerDialog->show();
            }
        }
    }
    if ( a == dataViewContextMenuCalc100to200 ) {
        QList<int> rows;
        Q_ASSERT (select != NULL);
        if ( select != NULL ) {
            //complete row is selected
            QModelIndexList sr = select->selectedRows();
            if ( ! sr.isEmpty() ) {
                //row 0: newest record (top) = dataList.last() !
                foreach ( QModelIndex i , select->selectedRows() ) {
                    rows.append( dataList.size() - i.row() - 1);
                }
            } else {
                //only 1 or more cells selected
                //row 0: newest record (top)
                QModelIndexList il = select->selectedIndexes();
                foreach (QModelIndex i, il) {
                    rows.append ( dataList.size() - i.row() - 1);
                }
            }
            if ( ! rows.isEmpty() ) {
                qSort (rows.begin(), rows.end());
                QMap<qreal,SpeedData> time = powerDialog->powerPlot()->calculateTimeBetweenSpeeds (dataList, rows, 100, 200);
                qDebug() << "100-200km/h in " << QString::number(time[-1].time_s, 'f',2) << " msecs";
                if ( time[-1].time_s > 0 ) {
                    QString s = "100-200 km/h in " + QString::number(time[-1].time_s, 'f', 2) + " secs (Geschwindigkeit aus GALA Signal von Tacho)";
                    s += "\nkm/h\ttime[s]\tt delta\talt delta\talt(GPS)";
                    foreach ( qreal sp , time.keys() ) {
                        if ( sp != -1 ) {
                            s += "\n" + QString::number(sp)
                                    + "\t" + QString::number(time[sp].time_s, 'f', 2)
                                    + "\t" + QString::number(time[sp].time_delta, 'f', 2)
                                    + "\t" + QString::number(time[sp].alt_delta, 'f', 2)
                                    + "\t" + QString::number(time[sp].alt, 'f', 2);
                        }
                    }

                    QMessageBox::information (NULL, "time 100-200 km/h",s);
                }
            }
        }
    }
    if ( a == dataViewContextMenuCalc100to200GPS ) {
        QList<int> rows;
        Q_ASSERT (select != NULL);
        if ( select != NULL ) {
            //complete row is selected
            QModelIndexList sr = select->selectedRows();
            if ( ! sr.isEmpty() ) {
                //row 0: newest record (top) = dataList.last() !
                foreach ( QModelIndex i , select->selectedRows() ) {
                    rows.append( dataList.size() - i.row() - 1);
                }
            } else {
                //only 1 or more cells selected
                //row 0: newest record (top)
                QModelIndexList il = select->selectedIndexes();
                foreach (QModelIndex i, il) {
                    rows.append ( dataList.size() - i.row() - 1);
                }
            }
            if ( ! rows.isEmpty() ) {
                qSort (rows.begin(), rows.end());
                QMap<qreal,SpeedData> time = powerDialog->powerPlot()->calculateTimeBetweenSpeedsGPS (dataList, rows, 100, 200);
                qDebug() << "100-200km/h in " << time[-1].time_s << " msecs";
                if ( time[-1].time_s > 0 ) {

                    QString s = "100-200 km/h in " + QString::number(time[-1].time_s, 'f', 2) + " secs (GPS)";
                    s += "\nkm/h\ttime[s]\tt delta\talt delta\talt(GPS)";
                    foreach ( qreal sp , time.keys() ) {
                        if ( sp != -1 )
                            s += "\n" + QString::number(sp)
                                    + "\t" + QString::number(time[sp].time_s, 'f', 2)
                                    + "\t" + QString::number(time[sp].time_delta, 'f', 2)
                                    + "\t" + QString::number(time[sp].alt_delta, 'f', 2)
                                    + "\t" + QString::number(time[sp].alt, 'f', 2);
                    }
                    QMessageBox::information (NULL, "time 100-200 km/h",s);
                }
            }
        }
    }

    if ( a==dataViewContextMenuSaveMarkedRows ) {
        //list is sorted
        QList<int> dr = helperGetUniqueRows ( select );
        qSort (dr.begin(), dr.end());
        qDebug() << dr;
//        qDebug() << "dataList.size() - dr.begin() - 1, dataList.size() - dr.end() - 1);
#if QT_VERSION >= 0x050000
        QString path = QStandardPaths::standardLocations (QStandardPaths::DocumentsLocation)[0]
                + QDir::separator() + "selection.mdv2";
#else
        QString path = QDesktopServices::storageLocation (QDesktopServices::DocumentsLocation)
                + QDir::separator() + "selection.mdv2";
#endif

        QString fn = QFileDialog::getSaveFileName ( NULL, QString("Select Filename"), path,
                                                    "mdv2 (*.mdv2)");
        saveData (fn, dataList.size() - dr.back() - 1, dataList.size() - dr.front() - 1);
    }

    if ( a == dataViewContextMenuFindWotEvents ) {
        findWot();
    }

    if ( a == dataViewContextMenuFindKnockEvents ) {
        findKnock();
    }
    if ( a == dataViewContextMenuFindHighEGTEvents ) {
        findHighEGT();
    }
    if ( a == dataViewContextMenuCheckEvents ) {
        checkData();
    }

}

void MdData::showDataListIdx (int i) {
    dataView->setCurrentIndex( createIndex( dataList.size() - i -1 , 0) );

    emit showRecordInVis1( dataList.size() - i - 1 );
}

QModelIndex MdData::findRowForMillis (quint32 millis) {
    int size = dataList.size();
    if ( size == 0 )
        return createIndex(0,0);
//        return createIndex(0,0,0);

    if ( millis > dataList.back()->getSensorR()->getTime() )
        return createIndex( size, 0);


    int lb = 0;
    int rb = size;
    int mi = size/2;
    bool found = false;
    int iterationen = 0;
    while ( !found ) {
        ++iterationen;
//        qDebug() << "lb=" << lb << " mi=" << mi << " rb=" << rb << " search=" << millis << " curVal="
//                 << dataList[mi]->getSensorR()->getTime() << " iteration " << iterationen;
        if ( ( dataList[mi]->getSensorR()->getTime() == millis )
//                || ( ( dataList[mi]->getSensorR()->getTime() > millis-10 ) &&
//                    ( dataList[mi]->getSensorR()->getTime() < millis+10 ) )
                || ( lb+1 == rb )
                ) {
            found = true;
        }
        else {
            if ( dataList[mi]->getSensorR()->getTime() < millis ) {
                lb = mi;
                mi = lb + (rb-lb)/2;
            }
            if ( dataList[mi]->getSensorR()->getTime() > millis ) {
                rb = mi;
                mi = lb + (rb-lb)/2;
            }
        }
    }
    qDebug() << "findRowForMillis " << millis << " index=" << mi << " " << dataList[mi]->getSensorR()->getTime();
    return createIndex( size-mi, 0);
}

int MdData::getLastTime () {
    if ( dataList.isEmpty() )
        return 0;
    return dataList.back()->getSensorR()->getTime();
}

void MdData::findWot () {
    QList <int> wotIdxL;
    #define STATE_NO_WOT 1
    #define STATE_WOT_START 2
    #define STATE_WOT_FOUND 3
    quint8 state = STATE_NO_WOT;
    int wot_start_time = 0;
    int wot_end_time = 0;
    int wot_start_idx = 0;

    for ( int i = 0 ; i < dataList.size() ; i++ ) {
        switch ( state ) {
            case STATE_NO_WOT:
            if ( dataList[i]->getSensorR()->getThrottle() >= 80 ) {
                if ( wot_end_time + 2000 > dataList[i]->getSensorR()->getTime() ) {
                    //delta between two WOT events too small -> discard the 2. wot event
                    //we have just a gear change here!
                   state = STATE_WOT_FOUND;
                } else {
                    state = STATE_WOT_START;
                    wot_start_time = dataList[i]->getSensorR()->getTime();
                    wot_start_idx = i;
                }
            }
            break;
            case STATE_WOT_START:
            if ( dataList[i]->getSensorR()->getThrottle() < 80 )
                state = STATE_NO_WOT;
            else {
                if ( wot_start_time + 2000 < dataList[i]->getSensorR()->getTime()  ) {
                    //2 secs wot
                    state = STATE_WOT_FOUND;
                    wotIdxL.append(wot_start_idx);
                }
            }
            break;
            case STATE_WOT_FOUND:
            if ( dataList[i]->getSensorR()->getThrottle() < 80 ) {
                state = STATE_NO_WOT;
                wot_end_time = dataList[i]->getSensorR()->getTime();
            }
            break;
        }
    }

    foreach ( int i, wotIdxL ) {
        qDebug() << "WOT event @ " << dataList[i]->getSensorR()->getTime() << " RPM=" << dataList[i]->getSensorR()->getRpm() << " bosot=" << dataList[i]->getSensorR()->getBoost();
    }
    wotEventsDialog->show( wotIdxL );
}

QList<int> MdData::findKnock(bool showWindow) {
    QList <int> knockIdxL;
    #define STATE_NO_KNOCK 1
    #define STATE_KNOCK 2
    #define STATE_KNOCK_FOUND 3
    quint8 state = STATE_NO_KNOCK;
    int knock_start_time = 0;
    int knock_end_time = 0;
    int knock_idx = 0;
    const qreal knock_threshold = 4;
    qreal knock_max = 0;

    for ( int i = 0 ; i < dataList.size() ; i++ ) {
        switch ( state ) {
            case STATE_NO_KNOCK:
            if ( dataList[i]->getSensorR()->df_ignition_total_retard >= knock_threshold ) {
                state = STATE_KNOCK;
                knock_start_time = dataList[i]->getSensorR()->getTime();
                knock_idx = i;
                knock_max = dataList[i]->getSensorR()->df_ignition_total_retard;
            }
            break;
            case STATE_KNOCK:
            if ( dataList[i]->getSensorR()->df_ignition_total_retard < knock_threshold ) {
                state = STATE_KNOCK_FOUND;

            } else {
                if ( knock_max < dataList[i]->getSensorR()->df_ignition_total_retard ) {
                    knock_max = dataList[i]->getSensorR()->df_ignition_total_retard;
                    knock_idx = i;
                }
            }
            break;
            case STATE_KNOCK_FOUND:
            knockIdxL.append(knock_idx);
            state = STATE_NO_KNOCK;
            break;
        }
    }

    foreach ( int i, knockIdxL ) {
        qDebug() << "Knock event @ " << dataList[i]->getSensorR()->getTime()
                 << " RPM=" << dataList[i]->getSensorR()->getRpm() << " boost="
                 << dataList[i]->getSensorR()->getBoost();
    }

    if ( showWindow )
        wotEventsDialog->showKnock ( knockIdxL );

    return knockIdxL;
}

QList<int> MdData::findHighEGT ( bool showWindow ) {
    QList <int> egtIdxL;
    #define STATE_LO 1
    #define STATE_HIGH 2
    #define STATE_HIGH_FOUND 3
    quint8 state = STATE_LO;
    int egt_time = 0;
    int egt_start_time = 0;
    int egt_idx = 0;
    const qreal egt_threshold = 950;
    qreal egt_max = 0;

    for ( int i = 0 ; i < dataList.size() ; i++ ) {
        switch ( state ) {
            case STATE_LO:
            if ( dataList[i]->getSensorR()->getHighestEgt()["temp"] >= egt_threshold ) {
                state = STATE_HIGH;
                egt_start_time = dataList[i]->getSensorR()->getTime();
                egt_idx = i;
                egt_max = dataList[i]->getSensorR()->getHighestEgt()["temp"];
            }
            break;
            case STATE_HIGH:
            if ( dataList[i]->getSensorR()->getHighestEgt()["temp"] < egt_threshold ) {
                state = STATE_HIGH_FOUND;

            } else {
                if ( egt_max < dataList[i]->getSensorR()->getHighestEgt()["temp"] ) {
                    egt_max = dataList[i]->getSensorR()->getHighestEgt()["temp"];
                    egt_idx = i;
                }
            }
            break;
            case STATE_HIGH_FOUND:
            egtIdxL.append(egt_idx);
            state = STATE_LO;
            break;
        }
    }

    foreach ( int i, egtIdxL ) {
        qDebug() << "High EGT event @ " << dataList[i]->getSensorR()->getTime() << " RPM="
                 << dataList[i]->getSensorR()->getRpm() << " boost=" << dataList[i]->getSensorR()->getBoost()
                 << " EGT=" << dataList[i]->getSensorR()->getHighestEgt()["temp"]
                 << " ("<< dataList[i]->getSensorR()->getHighestEgt()["idx"] << ")";
    }
    if ( showWindow )
        wotEventsDialog->showEGT ( egtIdxL );

    return egtIdxL;
}

QList<int> MdData::findInjectorHighDC ( bool showWindow ) {
    QList <int> dcIdxL;
    #define STATE_LO 1
    #define STATE_HIGH 2
    #define STATE_HIGH_FOUND 3
    quint8 state = STATE_LO;
    int time = 0;
    int start_time = 0;
    int idx = 0;
    const qreal dc_threshold = 90;
    qreal max = 0;

    for ( int i = 0 ; i < dataList.size() ; i++ ) {
        switch ( state ) {
            case STATE_LO:
            if ( dataList[i]->getSensorR()->df_inj_duty >= dc_threshold ) {
                state = STATE_HIGH;
                start_time = dataList[i]->getSensorR()->getTime();
                idx = i;
                max = dataList[i]->getSensorR()->df_inj_duty;
            }
            break;
            case STATE_HIGH:
            if ( dataList[i]->getSensorR()->df_inj_duty < dc_threshold ) {
                state = STATE_HIGH_FOUND;

            } else {
                if ( max < dataList[i]->getSensorR()->df_inj_duty ) {
                    max = dataList[i]->getSensorR()->df_inj_duty;
                    idx = i;
                }
            }
            break;
            case STATE_HIGH_FOUND:
            dcIdxL.append(idx);
            state = STATE_LO;
            break;
        }
    }

    foreach ( int i, dcIdxL ) {
        qDebug() << "High injector duty event " << dataList[i]->getSensorR()->df_inj_duty
                 << " @ " << dataList[i]->getSensorR()->getTime() << " RPM="
                 << dataList[i]->getSensorR()->getRpm() << " boost=" << dataList[i]->getSensorR()->getBoost()
                 << " EGT=" << dataList[i]->getSensorR()->getHighestEgt()["temp"]
                 << " ("<< dataList[i]->getSensorR()->getHighestEgt()["idx"] << ")";
    }
//    if ( showWindow )
//        wotEventsDialog->showEGT ( dcIdxL );

    return dcIdxL;
}

void MdData::checkData () {
    int eventCount = 0;

    //egt
    QList<int> egtL = findHighEGT( false );
    QList<QVariant> egtVL;
    foreach ( int i, egtL ) {
        egtVL.append( QVariant(i));
    }
    eventCount += egtL.size();

    QMap<QString,QMap<QString,QVariant > > p;
    QMap<QString,QVariant> edm;
    edm["data"] = egtVL;
    edm["icon"] = QVariant("dialog-warning");
    p["EGT"] = edm;

    //knock
    QList<int> knockL = findKnock( false );
    QList<QVariant> knockVL;
    foreach ( int i, knockL ) {
        knockVL.append( QVariant(i));
    }
    eventCount += knockL.size();

    QMap<QString,QVariant> km;
    km["data"] = knockVL;
    km["icon"] = QVariant("dialog-information");
    p["Knock"] = km;

    QList<int> dcL = findInjectorHighDC( false );
    QList<QVariant> dcVL;
    foreach ( int i, dcL ) {
        dcVL.append( QVariant(i));
    }
    eventCount += dcL.size();

    QMap<QString,QVariant> dcm;
    dcm["data"] = dcVL;
    dcm["icon"] = QVariant("dialog-warning");
    p["Inj duty cycle"] = dcm;

    if ( eventCount > 0 )
        wotEventsDialog->show ( p );
}

void MdData::showCfgVis1 () {
        visPlot->showCfgDialog();
}
void MdData::showCfgDataTable() {
    QMap<QString,bool> visible;
    for ( int i = 0 ; i < headerColNames.size() ; i++ ) {
        visible[headerColNames[i]] = ! dataView->isColumnHidden(i);
    }
    dataTableConfigDialog->readConfig(visible, headerColNames);
    dataTableConfigDialog->show();
}
void MdData::acceptCfgDataTable() {
    QTableWidget *tw = dataTableConfigDialog->getCfgTableWidget();

    for ( int i = 0 ; i < tw->rowCount() ; i++ ) {
        if ( (tw->item(i,1)->checkState()) == Qt::Checked) {
            dataView->setColumnHidden(i, false);
        } else {
            dataView->setColumnHidden(i, true);
        }
    }
}

void MdData::saveData () {
        saveData ("/tmp/test");
}

bool MdData::saveDataCSV ( const QString& filename ) {
        QFile file (filename);
        if ( !file.open (QIODevice::WriteOnly | QIODevice::Truncate) )
                return false;
        QTextStream ts (&file);

        //Header
        bool first = true;
        foreach (QString col, headerColNames) {
            if ( first ) {
                ts << col;
                first = false;
            } else
                ts << "\t" << col;
        }
        ts << "\n";


        for ( int r = 0 ; r < rowCount() ; ++r ) {
            first = true;
            for ( int c = 0 ; c < columnCount() ; ++c ) {
                if ( first ) {
                    ts << data(QAbstractItemModel::createIndex(r,c)).toString();
                    first = false;
                } else
                    ts << "\t" << data(QAbstractItemModel::createIndex(r,c)).toString();
            }
            ts << "\n";
        }

        file.close();
        emit showStatusMessage ("CSV data exported to File " + filename);
        return true;
}

bool MdData::saveData ( const QString& filename, int begin, int end ) {
        QFile file (filename);
        if ( !file.open (QIODevice::WriteOnly | QIODevice::Truncate) )
                return false;
        QDataStream ds (&file);
        ds.setVersion(QDataStream::Qt_4_6);
        ds << MAGICNUMBER;
        ds << VERSION4;

        int l = 0;
        if ( begin == 0 && end == 0 ) {
            foreach ( MdDataRecord* dr, dataList ) {
                    ds << dr;
                    ++l;
            }
        } else {
            if ( begin < (dataList.size()-1) && (end <= dataList.size()-1) ) {
                //save only selected rows
                for ( int i=begin; i<= end ; i++ ) {
                    ds << dataList[i];
                }
            } else {
                qDebug() << "saveData out of bounds begin=" << begin << " end=" << end;
            }
        }
        file.close();
        emit showStatusMessage ("Data saved to File " + filename + " (" + QString::number(l) + " rows)");
        return true;
}

bool MdData::loadData ( const QString& filename ) {
	clearData ();

	QFile file (filename);
		if ( !file.open (QIODevice::ReadOnly) )
			return false;
	QDataStream ds (&file);
	ds.setVersion(QDataStream::Qt_4_6);
	quint32 magic, version;
	ds >> magic;
	ds >> version;

    int l = 0;
	MdDataRecord* r = NULL;
        Map16x1_Voltage vmap;

        switch ( version ) {
        case CVERSION1:
            while ( !ds.atEnd() ) {
                compatibility::MdDataRecordV1* rc = new compatibility::MdDataRecordV1();
                ds >> rc;
                r = rc;
                if ( r ) {
                    addDataRecord(r,false);
                    ++l;
                }
            }
            break;
        case CVERSION3:
            qDebug ("CVERSION3");
            while ( !ds.atEnd() ) {
                compatibility::MdDataRecordV2* rc = new compatibility::MdDataRecordV2();
                ds >> rc;
                r = rc;
                r->getSensorR()->df_voltage = vmap.mapValue(r->getSensorR()->df_voltage_raw);
                if ( r ) {
                    addDataRecord(r,false);
                    ++l;
                }
            }
            break;
        case VERSION4:
            qDebug ("VERSION4");
            while ( !ds.atEnd() ) {
                r = new MdDataRecord();
                ds >> r;
                if ( r ) {
                    addDataRecord(r,false);
                    ++l;
                }
            }
            break;
        default:
            QMessageBox::critical  ( NULL, QString("wrong file version"),
                                     QString("load of incompatible file version ")  + QString::number(version) + QString(" attempted!") );
            return false;
        }


//	while ( !ds.atEnd() ) {
//		if ( version == VERSION ) {
//			r = new MdDataRecord();
//			ds >> r;
//		} else if ( version == CVERSION1 ) {
//			compatibility::MdDataRecordV1* rc = new compatibility::MdDataRecordV1();
//			ds >> rc;
//			r = rc;
//		} else {
//			QMessageBox::critical  ( NULL, QString("wrong file version"),
//                                                 QString("load of incompatible file version ")  + QString::number(version) + QString(" attempted!") );
//			return false;
//		}
//		if ( r ) {
//			addDataRecord (r, false);
//		}
//	}
	file.close();
	replot();

    emit showStatusMessage ("Data loaded from File " + filename + " (" + QString::number(l) + " rows)");

    //new: CHECK it!
    checkData();
    dataView->resizeColumnsToContents();

	return true;
}

void MdData::clearData () {
//	qDebug() << "clear " << data.size() << " items";
//	while ( !dataList.empty() ) {
//		delete (*(dataList.begin()));
//		dataList.pop_front();
//	}
//	qDebug() << "size=" << data.size();

	foreach ( MdPlot* p, plotList) {
		p->clear();
	}

	foreach ( MdDataRecord* r , dataList )
			delete (r);
	dataList.clear();
}

void MdData::addDataRecord (MdDataRecord *nr, bool doReplot) {
	insertRows(dataList.size(), 1, QModelIndex());
	dataList.push_back(nr);
    if ( dataView ) {
//        dataView->resizeRowsToContents();
//        dataView->resizeColumnsToContents();
        ;
    }
	visualizeDataRecord(nr, doReplot);
}

void MdData::checkMaxValues (MdDataRecord* nr) {

}

//neu *************************************************
void MdData::clearPlots() {
    foreach ( MdPlot* p, plotList) {
        p->clear();
    }
}

int MdData::rowCount(const QModelIndex & parent) const {
	Q_UNUSED(parent);
	return dataList.size();
}
int MdData::columnCount(const QModelIndex & parent) const {
	Q_UNUSED(parent);
    return headerColNames.size();
//    qDebug()<<"columnCount " << mapVisableColsToindex.columns.size();
}

QVariant MdData::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if ( section < headerColNames.size() )
            return QString (headerColNames.at(section));
    }
	return QVariant();
}

QVariant MdData::data(const QModelIndex & index, int role) const {
//	qDebug() << "MdData::data index=" << index << " role=" << role;


    if (!index.isValid())
        return QVariant();

    if (index.row() >= (int) dataList.size())
        return QVariant();

    if ( role == Qt::ToolTipRole ){
        int r = dataList.size() - index.row() - 1;
        qDebug() << "toolTip col=" << index.column() << " row=" << index.row();
        switch (index.column()) {
            case 29: if ( dataList.at(r)->getColumn(index.column()).toDouble() > 0 ) {
                //knock
                QString tip = "cyl retard: " + QString::number( dataList.at(r)->getSensorR()->df_cyl1_knock_retard * 0.351563, 'f', 1) + "|"
                        + QString::number( dataList.at(r)->getSensorR()->df_cyl2_knock_retard * 0.351563, 'f', 1) + "|"
                        + QString::number( dataList.at(r)->getSensorR()->df_cyl3_knock_retard * 0.351563, 'f', 1) + "|"
                        + QString::number( dataList.at(r)->getSensorR()->df_cyl4_knock_retard * 0.351563, 'f', 1);
                tip += "\n";
                tip += "decay: " + QString::number( dataList.at(r)->getSensorR()->df_cyl1_knock_decay ) + "|"
                        + QString::number( dataList.at(r)->getSensorR()->df_cyl2_knock_decay ) + "|"
                        + QString::number( dataList.at(r)->getSensorR()->df_cyl3_knock_decay ) + "|"
                        + QString::number( dataList.at(r)->getSensorR()->df_cyl4_knock_decay );

                return QVariant(tip);
            }
            case 0: {
                //time
                if ( index.row() > 1 ) {
                    MdDataRecord *cur = dataList.at(r);
                    MdDataRecord *last = dataList.at(r-1);
                    //use gps data?
                    qreal start_velocity = last->getSensorR()->getSpeed();
                    qreal end_velocity = cur->getSensorR()->getSpeed();
                    qreal time_s = (cur->getSensorR()->getTime() - last->getSensorR()->getTime()) / 1000.0;
                    qreal mass = 1150;
                    //m/s^2
                    //use accelerometer data from N900 ?
                    qreal acceleration = (end_velocity - start_velocity) * (0.27778) / (time_s);
                    qreal force = mass * acceleration;
                    //distance travelled
                    qreal distance = (start_velocity * 0.27778 * time_s) +  1/2 * acceleration * time_s * time_s;
                    //work
                    qreal work = force * distance;
                    qreal power = work / time_s;
//                    qDebug() << "start time=" << last->getSensorR()->getTime() << " end time=" << cur->getSensorR()->getTime();
//                    qDebug() << "start velocity " << QString::number(start_velocity, 'f', 4) << " end_velocity=" << QString::number(end_velocity, 'f', 4) << " time=" << QString::number(time_s, 'g', 2)
//                                << " distance=" << QString::number(distance, 'g', 2) << " acceleration=" << QString::number(acceleration, 'g', 2) << " force=" << QString::number(force, 'g', 2) << " worK="
//                                << QString::number(work, 'f', 2) << " power=" << QString::number(power, 'f', 2);

                    //luftwiderstand
                    /* Fr = A/2 * Cw * D * v�
                    A=Stirnfl�che, Cw=Cw Wert, D=Dichte der Luft (abh�ngig von Temperatur)
                    cw-Wert bei 195er 0,32 bei 205er und Plusachse 0,34 (ist aber nicht sicher!!)
                    Stirnfl�chen: normaler Corrado 1,81m� US und 2.0L (kleine Frontlippe) 1,80m�
                    */
                    qreal f_air = 1.81*0.5 * 0.34 * 1.29 * qPow(end_velocity*0.27778, 2);
                    qreal w_air = f_air * distance;
                    qreal p_air = w_air / time_s;

                    /*
                      Rollwiderstand
                    Fr = �r * m * g
                    [N]=[ ] [kg] [m/s�]

                    �r = Rollwiderstandsbeiwert
                    Rollwiderstand 0,015 drin 215/40
                    195/50 Rollwiderstand 0,013

                    gueltig auch bei hohen Geschwindigkeiten > 150km/h???
                    */
                    qreal f_roll = 0.015 * mass * 9.81;
                    qreal w_roll = f_roll * distance;
                    qreal p_roll = w_roll / time_s;

                    qreal wpower = power + p_roll + p_air;
                    qreal epower = wpower * (1/.763);
                    //Korrekturfaktor = (1013 / Atmosph�rendruck) x Wurzel aus [(273 + Temperatur am Pr�fstand) / (273 + 20)]
                    qreal temp = 20;
                    qreal luftdruck = 1013;
                    qreal dinpower = epower * ((1013 / luftdruck) * qSqrt ((273 + temp) / (273 + 20)));
                    qreal torque = (dinpower)/(2*M_PI * (cur->getSensorR()->getRpm()/60) );

//                    QString tip = "Power " + QString::number(power/1000, 'f', 2) + " (kW) " + QString::number( (power*1.34)/1000, 'f', 2 ) + " (hp/PS)";
//                    qDebug() << tip;
                    QString tip2 = "Power (wheel) " + QString::number( wpower/1000, 'f', 2)
                            + " (kW) " + QString::number( (wpower*1.34)/1000, 'f', 2 ) + " (Hp/PS)"
                            + "\n" + "Engine Power "  + QString::number( epower/1000, 'f', 2)
                            + " (kW) " + QString::number( (epower*1.34)/1000, 'f', 2 ) + " (Hp/PS)"
                            + "\n" + "Engine Power DIN 70020 "  + QString::number( dinpower/1000, 'f', 2)
                            + " (kW) " + QString::number( (dinpower*1.34)/1000, 'f', 2 ) + " (Hp/PS)" + " " + QString::number(torque, 'f', 2) + "Nm";
                    qDebug() << tip2;
                    return QVariant (tip2);
                }
                break;
            }

            case 1: {
                //RPM
                QString s = QString::number (dataList.at(r)->getSensorR()->df_rpm_delta_hall);
                return QVariant ( s );
            }
            //DF lambda debug
            case 43:  {
            /*
              df_cyl4_knock_decay = b_58_oxs_pause
              df_col
              */
                QString res = "Lambda = " + QString::number( dataList.at(r)->getSensorR()->getLambda(), 'f', 2 ) + "\n";
                res += "oxs_pause = " + QString::number( dataList.at(r)->getSensorR()->df_cyl4_knock_decay ) + "\n";
                res += "oxs_P_timer " + QString::number( (qint8) dataList.at(r)->getSensorR()->df_cold_startup_enrichment)  + "\n";
                res += "oxs_I_timer " + QString::number( (qint8) dataList.at(r)->getSensorR()->df_warm_startup_enrichment)  + "\n";
                quint16 oxs_i_comp = (dataList.at(r)->getSensorR()->df_ect_enrichment << 8) + dataList.at(r)->getSensorR()->df_ect_injection_addon;
                res += "oxs P = " + QString::number( (qint8) dataList.at(r)->getSensorR()->df_cyl2_knock_decay )  + "\n";
                res += "oxs I = " + QString::number( (qint8) dataList.at(r)->getSensorR()->df_cyl3_knock_decay )  + "\n";
                res += "oxs I comp = " + QString::number( (qint16) oxs_i_comp )  + "\n";

                res += "Flags = ";
                quint8 oxs_state = (dataList.at(r)->getSensorR()->df_flags & 0x60) >> 5;
                QString oxs_state_str = QString::number( oxs_state ) + " | ";
                if ( oxs_state == 0)
                    oxs_state_str += "LEAN";
                if ( oxs_state == 2)
                    oxs_state_str += "RICH";
                if ( oxs_state == 3)
                    oxs_state_str += "STOICH";
                res += oxs_state_str;
                if ( ((dataList.at(r)->getSensorR()->df_flags) & 0x2) == 2 )
                    res += " | 2";
                else
                    res += " | !2";

                return QVariant(res);

                break;
            }

        }


        return QVariant();
    }
    if (role == Qt::DisplayRole) {
    	int r = dataList.size() - index.row() - 1;

        if (r < 0)
            return QVariant();

        //Time
        if ( index.column() == 0 ) {
            QTime t = QTime(0, 0, 0, 0);
            t = t.addMSecs( dataList.at(r)->getColumn(index.column()).toInt() );
            return QVariant (t.toString("hh:mm:ss.zzz"));
        }
        //Throttle
        if ( index.column() == 3 ) {
            QString res = dataList.at(r)->getColumn(index.column()).toString() + " | ";
//            QString res = QString::number(dataList.at(r)->getSensorR()->df_cyl1_knock_decay) + " | ";
            if ( dataList.at(r)->getSensorR()->df_flags & 8 )
                res += "WOT";
            else if ( dataList.at(r)->getSensorR()->df_flags & 0x10 )
                res += "Idle";
            return QVariant(res);
        }
        //speed
        if ( index.column() == 22 ) {
            QString res = dataList.at(r)->getColumn(index.column()).toString();
            //compute delta
            int last = dataList.size() - index.row() - 2;
            if (  last >= 0 ) {
                double speedDelta = dataList.at(r)->getSensorR()->getSpeed() - dataList.at(last)->getSensorR()->getSpeed();
                res += " (" + QString::number(speedDelta, 'f', 1) + ")";
            }
            if (  last >= 0 ) {
                double gpsSpeedDelta = dataList.at(r)->getMobileR()->gpsGroundSpeed - dataList.at(last)->getMobileR()->gpsGroundSpeed;
                res += " ( " +  QString::number(dataList.at(r)->getMobileR()->gpsGroundSpeed) + "/" + QString::number(gpsSpeedDelta, 'f', 1) + " / "  + QString::number(dataList.at(r)->getMobileR()->gpsUpdateCount) + " GPS)";
            }

            return QVariant(res);
        }
        //df injection time
        if ( index.column() == 30 ) {
            qreal df_inj_time_ms = dataList.at(r)->getColumn(index.column()).toDouble() / 1000.0;

            QString res = QString::number( df_inj_time_ms, 'f', 1 ) + " ms ";
            res += QString::number( dataList.at(r)->getSensorR()->df_inj_duty, 'f', 1 ) + "%";
            return QVariant(res);
        }
        //DF lambda
        if ( index.column() == 43 ) {
            QString res = dataList.at(r)->getColumn(index.column()).toString();
            res += " " + QString::number( (qint8) dataList.at(r)->getSensorR()->df_cyl2_knock_decay );
            res += " " + QString::number( (qint8) dataList.at(r)->getSensorR()->df_cyl3_knock_decay );
            return QVariant(res);
        }
        //LC Flags
        if ( index.column() == 46 ) {
            QString res = dataList.at(r)->getColumn(index.column()).toString();
            if ( dataList.at(r)->getSensorR()->df_lc_flags & 32 )
                res += " | C";

            if ( (dataList.at(r)->getSensorR()->df_lc_flags & 3) == 3)
                res += " | sWait";
            else {
                if ( dataList.at(r)->getSensorR()->df_lc_flags & 1)
                    res += " | LC";
                else if ( dataList.at(r)->getSensorR()->df_lc_flags & 4)
                        res += " | WOTs";
            }

            if ( dataList.at(r)->getSensorR()->df_lc_flags & 16 )
                res += " | K_off";
            return QVariant(res);
        }
        //RPM test
//        if ( index.column() == 1 ) {
//            return dataList.at(r)->getColumn(index.column()).toString() + " | " + QString::number( rpmMap->mapValue( dataList.at(r)->getSensorR()->df_rpm_map ) );
//        }
        //VDO pressure mbar to bar
        if ( index.column() == 16 || index.column() == 17 || index.column() == 18 ) {
            return QVariant (dataList.at(r)->getColumn(index.column()).toDouble() / 1000);
        }
        //VDO temp
//        if ( index.column() == 19 || index.column() == 20 || index.column() == 21 ) {
//            return QVariant (dataList.at(r)->getColumn(index.column())) ;
//        }

        //DF ISV
        if ( index.column() == 45 ) {
            return QVariant (( 0x1a93 - ( (( isvMap->mapValue( dataList.at(r)->getColumn(index.column()).toInt() ) * 0xC7 ) / 16 ) + 0xA60 ) ) / 2 );
        }
        return dataList.at(r)->getColumn(index.column());
    }

    if (role == Qt::BackgroundColorRole) {
        int r = dataList.size() - index.row() - 1;
        switch ( index.column() ) {
            case 3:
            case 0: //time. mark it cyan to indicate throttle > 80%
                    if ( dataList.at(r)->getColumn(3).toInt() > 80 )
                        return QColor(Qt::cyan);
                    break;
            case 1:
                if ( index.column() == 1 ) {
                    //RPM
                    if ( dataList.at(r)->getSensorR()->df_flags & 8 ) {
                        if ( r > 1 )
                            if ( dataList.at(r-1)->getSensorR()->getRpm() >  dataList.at(r)->getSensorR()->getRpm() )
                                //should not happen on wot
                                return QColor(Qt::red);
                    }
                }
                break;
            case 4: //Lambda
                    return lambdaBlend->overblend3( dataList.at(r)->getSensorR()->getLambda() );
                    break;
                //EGT
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
                        if ( dataList.at(r)->getColumn(index.column()).toDouble() == 0 )
                            return QColor(Qt::white);
                        if ( dataList.at(r)->getColumn(index.column()).toDouble() > 1600 )
                            return QColor(Qt::white);
                        return egtBlend->overblend3( dataList.at(r)->getColumn(index.column()).toDouble() );
                        break;
            case 27: //efr speed
                        return efrBlend->overblend3( dataList.at(r)->getColumn(index.column()).toDouble() );
                        break;
            case 2: //boost
                        return boostBlend->overblend3( dataList.at(r)->getColumn(index.column()).toDouble() );
                        break;
            case 29: //knock retard
                        return knockBlend->overblend3( dataList.at(r)->getColumn(index.column()).toDouble() );
                        break;
            case 30: //df injection
                    return injectorDutyBlend ->overblend3( dataList.at(r)->getSensorR()->df_inj_duty );
                    break;
            case 33: //raw knock
                        return rawKnockBlend->overblend3( dataList.at(r)->getColumn(index.column()).toDouble() );
            case 40: //df voltage
            case 15: //md voltage
                        if ( dataList.at(r)->getColumn(index.column()).toDouble() < 8 )
                            return QColor(Qt::white);
                        return voltageBlend->overblend3( dataList.at(r)->getColumn(index.column()).toDouble() );
                        break;
            case 31: //df iat
                        return iatBlend->overblend3( dataList.at(r)->getColumn(index.column()).toDouble() );
                        break;
                        //GPS check if valid
            case 43: //df lambda
                        return lambdaBlend->overblend3( dataList.at(r)->getSensorR()->getLambda() );
                        break;
            case 48:
            case 49:
            case 50:
                if ( dataList.at(r)->getMobileR()->gpsValid == false )
                    return QColor(Qt::red);
                break;
        }
    }

	return QVariant();
}

bool MdData::insertRows(int position, int rows, const QModelIndex &parent) {
	Q_UNUSED(parent);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    endInsertRows();
    return true;
}


bool MdData::removeRows ( int row, int count, const QModelIndex & parent ) {
//    qDebug() << "MdData::removeRows row=" << row << " count=" << count << " dataList.size())=" << dataList.size() ;
    Q_UNUSED(parent);
    beginRemoveRows(QModelIndex(), row, row + count -1 );
    for ( int i = row-1 ; i < row + count - 1 ; i++ )
        dataList.removeAt(i);
    endRemoveRows();
    return true;
}

void MdData::visualizeDataRecord (MdDataRecord* nr, bool doReplot) {
    if ( nr->getSensorR() != NULL ) {
        boostPidPlot->addRecord(nr->getSensorR(), doReplot );
    }
    if ( nr->getSensorR() != NULL ) {
        visPlot->addRecord(nr->getSensorR(), doReplot );
    }
    emit rtNewDataRecord(nr);

	//TODO only if tab is active!
	//replot gets called in addREcord
//	if ( doReplot ) {
//		visPlot->replot();
//		boostPidPlot->replot();
//	}
}

QList<MdDataRecord*> & MdData::getData() {
	return dataList;
}

int MdData::size() {
	return dataList.size();
}

void MdData::replot() {
	foreach ( MdPlot* p, plotList)
			p->replot();
}


VisualizationPlot* MdData::getVisPlot() {
	return visPlot;
}

int MdData::changeDataWinMarkToDisplayRecord (const int &element, const int &maxMark) {
    double recordsPerMark = round ( (double) dataList.size() / (double) maxMark );
    int nm = (int) ceil ( element / recordsPerMark);
    changeDataWinMark(nm, maxMark);
    return nm;
}

void MdData::changeDataWinMarkMicroRelative (const int &quotient, const bool &left, const int &maxMark) {
	foreach ( MdPlot* p, plotList ) {
		p->setWinMarkMicroRelative(quotient, left, maxMark);
		p->replot();
	}
}

void MdData::changeDataWinMark (const int &nm, const int &maxMark) {
//        qDebug() << "changeDataWinMark new value=" << nm << " maxMark=" << maxMark;
	foreach ( MdPlot* p, plotList ) {
		p->setWinMark(nm ,maxMark);
		p->replot();
	}
}
void MdData::changeDataWinSize (const int &ns) {
//	qDebug() << "changeDataWinSize new value=" << ns;
	foreach ( MdPlot* p, plotList ) {
		p->setWinSize(ns);
		p->replot();
	}

}

void MdData::toggleZoomMode() {
	visPlot->toggleZoomMode();
}




MdSensorRecord::MdSensorRecord() {
    efr_speed = 0;
    knock = 0;
    df_kline_framenum = 255;
    df_kline_freq = 0;
}

MdSensorRecord::MdSensorRecord ( int time, int rpm, int throttle, double boost, double lambda, double lmm,
                 double casetemp, double agt0, double agt1, double agt2, double agt3, double agt4,
                 double agt5, double agt6, double agt7, double batcur,
                 double VDOPres1, double VDOPres2, double VDOPres3,
                 double VDOTemp1, double VDOTemp2, double VDOTemp3,
                 double speed, int gear, int n75, double n75_req_boost, quint8 n75_req_boost_pwm, quint8 flags,
                 double efr_speed,
                 quint8 df_boost_raw, quint8 df_lambda, quint8 df_knock_raw, quint8 df_ect_raw, quint8 df_iat_raw,
                 quint8 df_co_poti, quint8 df_flags, quint8 df_ign,
                 quint8 df_cyl1_knock_retard, quint8 df_cyl1_knock_decay, quint8 df_cyl2_knock_retard,
                 quint8 df_cyl2_knock_decay, quint8 df_cyl3_knock_retard, quint8 df_cyl3_knock_decay, quint8 df_cyl4_knock_retard,
                 quint8 df_cyl4_knock_decay, quint8 df_voltage_raw, quint16 df_inj_time,
                 quint8 df_cold_startup_enrichment, quint8 df_warm_startup_enrichment, quint8 df_ect_startup_enrichment,
                 quint8 df_acceleration_enrichment, quint8 df_counter_startup_enrichment, quint8 df_iat_startup_enrichment,
                 quint8 df_ignition_addon_counter, quint8 df_igniton_addon, quint8 df_ect_injection_addon,
                 quint16 df_rpm_delta_hall, quint8 df_isv, quint8 df_lc_flags,
                 double df_ignition_total_retard, double df_ect, double df_iat, double df_ignition, double df_voltage,
                 quint16 knock, quint16 df_kline_freq, quint8 df_kline_framenum
                 ) :
          time(time), rpm(rpm), throttle(throttle), boost(boost), lambda(lambda), lmm(lmm), casetemp(casetemp),
          batcur(batcur), VDOPres1(VDOPres1), VDOPres2(VDOPres2), VDOPres3(VDOPres3), VDOTemp1(VDOTemp1), VDOTemp2(VDOTemp2), VDOTemp3(VDOTemp3),
          speed (speed), gear(gear), n75(n75), n75_req_boost(n75_req_boost), n75_req_boost_pwm(n75_req_boost_pwm), flags(flags),
          efr_speed(efr_speed),
          df_boost_raw(df_boost_raw), df_lambda(df_lambda), df_knock_raw(df_knock_raw), df_ect_raw(df_ect), df_iat_raw(df_iat), df_co_poti(df_co_poti), df_flags(df_flags), df_ign(df_ign),
          df_cyl1_knock_retard(df_cyl1_knock_retard), df_cyl1_knock_decay(df_cyl1_knock_decay), df_cyl2_knock_retard(df_cyl2_knock_retard), df_cyl2_knock_decay(df_cyl2_knock_decay),
          df_cyl3_knock_retard(df_cyl3_knock_retard), df_cyl3_knock_decay(df_cyl3_knock_decay), df_cyl4_knock_retard(df_cyl4_knock_retard), df_cyl4_knock_decay(df_cyl4_knock_decay),
          df_voltage_raw(df_voltage_raw), df_inj_time(df_inj_time), df_cold_startup_enrichment(df_cold_startup_enrichment), df_warm_startup_enrichment(df_warm_startup_enrichment),
          df_ect_enrichment(df_ect_startup_enrichment), df_acceleration_enrichment(df_acceleration_enrichment), df_counter_startup_enrichment(df_counter_startup_enrichment), df_iat_enrichment(df_iat_startup_enrichment),
          df_ignition_addon_counter(df_ignition_addon_counter), df_igniton_addon(df_igniton_addon), df_ect_injection_addon(df_ect_injection_addon),
          df_rpm_delta_hall(df_rpm_delta_hall),  df_isv(df_isv), df_lc_flags(df_lc_flags),
          df_ignition_total_retard(df_ignition_total_retard), df_ect(df_ect), df_iat(df_iat), df_ignition(df_ignition), df_voltage(df_voltage),
          knock (knock), df_kline_freq(df_kline_freq), df_kline_framenum(df_kline_framenum)

{
    egt[0]=agt0;
    egt[1]=agt1;
    egt[2]=agt2;
    egt[3]=agt3;
    egt[4]=agt4;
    egt[5]=agt5;
    egt[6]=agt6;
    egt[7]=agt7;
    df_inj_duty = 2* (( (df_inj_time / 1000.0) * rpm)/1200.0);
}

MdSensorRecord::MdSensorRecord ( const MdSensorRecord& d ) {
    time = d.time;
    rpm = d.rpm;
    throttle = d.throttle;
    boost = d.boost;
    lambda = d.lambda;
    lmm = d.lmm;
    casetemp = d.casetemp;
    egt[0] = d.egt[0];
    egt[1] = d.egt[1];
    egt[2] = d.egt[2];
    egt[3] = d.egt[3];
    egt[4] = d.egt[4];
    egt[5] = d.egt[5];
    egt[6] = d.egt[6];
    egt[7] = d.egt[7];
    batcur = d.batcur;
    VDOPres1 = d.VDOPres1;
    VDOPres2 = d.VDOPres2;
    VDOPres3 = d.VDOPres3;
    VDOTemp1 = d.VDOTemp1;
    VDOTemp2 = d.VDOTemp2;
    VDOTemp3 = d.VDOTemp3;
    speed = d.speed;
    gear = d.gear;
    n75 = d.n75;
    n75_req_boost = d.n75_req_boost;
    n75_req_boost_pwm = d.n75_req_boost_pwm;
    flags = d.flags;
    efr_speed = d.efr_speed;
    df_boost_raw = d.df_boost_raw;
    df_lambda = d.df_lambda;
    df_knock_raw = d.df_knock_raw;
    df_ect_raw = d.df_ect_raw;
    df_iat_raw = d.df_iat_raw;
    df_co_poti = d.df_co_poti;
    df_flags = d.df_flags;
    df_ign = d.df_ign;
    df_cyl1_knock_retard = d.df_cyl1_knock_retard;
    df_cyl1_knock_decay = d.df_cyl1_knock_decay;
    df_cyl2_knock_retard = d.df_cyl2_knock_retard;
    df_cyl2_knock_decay = d.df_cyl2_knock_decay;
    df_cyl3_knock_retard = d.df_cyl3_knock_retard;
    df_cyl3_knock_decay = d.df_cyl3_knock_decay;
    df_cyl4_knock_retard = d.df_cyl4_knock_retard;
    df_cyl4_knock_decay = d.df_cyl4_knock_decay;
    df_voltage_raw = d.df_voltage_raw;
    df_inj_time = d.df_inj_time;
    df_cold_startup_enrichment = d.df_cold_startup_enrichment;
    df_warm_startup_enrichment = d.df_warm_startup_enrichment;
    df_ect_enrichment = d.df_ect_enrichment;
    df_acceleration_enrichment = d.df_acceleration_enrichment;
    df_counter_startup_enrichment = d.df_counter_startup_enrichment;
    df_iat_enrichment = d.df_iat_enrichment;
    df_ignition_addon_counter = d.df_ignition_addon_counter;
    df_igniton_addon = d.df_igniton_addon;
    df_ect_injection_addon = d.df_ect_injection_addon;
    df_rpm_delta_hall = d.df_rpm_delta_hall;
    df_isv = d.df_isv;
    df_lc_flags = d.df_lc_flags;
    df_ignition_total_retard = d.df_ignition_total_retard;
    df_ect = d.df_ect;
    df_iat = d.df_iat;
    df_ignition = d.df_ignition;
    knock = d.knock;
    df_kline_freq = d.df_kline_freq;
    df_kline_framenum = d.df_kline_framenum;

    //some computations
    df_inj_duty = 2* (( (df_inj_time / 1000.0) * rpm)/1200.0);
}

MdSensorRecord::~MdSensorRecord() {
}

QDataStream& operator<< (QDataStream& s, MdSensorRecord *r) {
    s << r->time;
    s << r->rpm;
    s << r->throttle;
    s << r->boost;
    s << r->lambda;
    s << r->lmm;
    s << r->casetemp;
    s << r->egt[0];
    s << r->egt[1];
    s << r->egt[2];
    s << r->egt[3];
    s << r->egt[4];
    s << r->egt[5];
    s << r->egt[6];
    s << r->egt[7];
    s << r->batcur;
    s << r->VDOPres1;
    s << r->VDOPres2;
    s << r->VDOPres3;
    s << r->VDOTemp1;
    s << r->VDOTemp2;
    s << r->VDOTemp3;
    s << r->speed;
    s << r->gear;
    s << r->n75;
    s << r->n75_req_boost;
    s << r->n75_req_boost_pwm;
    s << r->flags;
    s << r->efr_speed;

    s << r->df_boost_raw;
    s << r->df_lambda;
    s << r->df_knock_raw;
    s << r->df_ect_raw;
    s << r->df_iat_raw;
    s << r->df_co_poti;
    s << r->df_flags;
    s << r->df_ign;
    s << r->df_cyl1_knock_retard;
    s << r->df_cyl1_knock_decay;
    s << r->df_cyl2_knock_retard;
    s << r->df_cyl2_knock_decay;
    s << r->df_cyl3_knock_retard;
    s << r->df_cyl3_knock_decay;
    s << r->df_cyl4_knock_retard;
    s << r->df_cyl4_knock_decay;
    s << r->df_voltage_raw;
    s << r->df_inj_time;
    s << r->df_cold_startup_enrichment;
    s << r->df_warm_startup_enrichment;
    s << r->df_ect_enrichment;
    s << r->df_acceleration_enrichment;
    s << r->df_counter_startup_enrichment;
    s << r->df_iat_enrichment;
    s << r->df_ignition_addon_counter;
    s << r->df_igniton_addon;
    s << r->df_ect_injection_addon;
    s << r->df_rpm_delta_hall;
    s << r->df_isv;
    s << r->df_lc_flags;

    s << r->df_ignition_total_retard;
    s << r->df_ect;
    s << r->df_iat;
    s << r->df_ignition;
    s << r->df_voltage;

    s << r->knock;
    s << r->df_kline_freq;
    s << r->df_kline_framenum;

    return s;
}
QDataStream& operator>> (QDataStream& s, MdSensorRecord *r) {
    s >> r->time;
    s >> r->rpm;
    s >> r->throttle;
    s >> r->boost;
    s >> r->lambda;
    s >> r->lmm;
    s >> r->casetemp;
    s >> r->egt[0];
    s >> r->egt[1];
    s >> r->egt[2];
    s >> r->egt[3];
    s >> r->egt[4];
    s >> r->egt[5];
    s >> r->egt[6];
    s >> r->egt[7];
    s >> r->batcur;
    s >> r->VDOPres1;
    s >> r->VDOPres2;
    s >> r->VDOPres3;
    s >> r->VDOTemp1;
    s >> r->VDOTemp2;
    s >> r->VDOTemp3;
    s >> r->speed;
    s >> r->gear;
    s >> r->n75;
    s >> r->n75_req_boost;
    s >> r->n75_req_boost_pwm;
    s >> r->flags;
    s >> r->efr_speed;

    s >> r->df_boost_raw;
    s >> r->df_lambda;
    s >> r->df_knock_raw;
    s >> r->df_ect_raw;
    s >> r->df_iat_raw;
    s >> r->df_co_poti;
    s >> r->df_flags;
    s >> r->df_ign;
    s >> r->df_cyl1_knock_retard;
    s >> r->df_cyl1_knock_decay;
    s >> r->df_cyl2_knock_retard;
    s >> r->df_cyl2_knock_decay;
    s >> r->df_cyl3_knock_retard;
    s >> r->df_cyl3_knock_decay;
    s >> r->df_cyl4_knock_retard;
    s >> r->df_cyl4_knock_decay;
    s >> r->df_voltage_raw;
    s >> r->df_inj_time;
    s >> r->df_cold_startup_enrichment;
    s >> r->df_warm_startup_enrichment;
    s >> r->df_ect_enrichment;
    s >> r->df_acceleration_enrichment;
    s >> r->df_counter_startup_enrichment;
    s >> r->df_iat_enrichment;
    s >> r->df_ignition_addon_counter;
    s >> r->df_igniton_addon;
    s >> r->df_ect_injection_addon;
    s >> r->df_rpm_delta_hall;
    s >> r->df_isv;
    s >> r->df_lc_flags;

    s >> r->df_ignition_total_retard;
    s >> r->df_ect;
    s >> r->df_iat;
    s >> r->df_ignition;
    s >> r->df_voltage;

    s >> r->knock;
    s >> r->df_kline_freq;
    s >> r->df_kline_framenum;

    //some computations
    r->df_inj_duty = 2* (( (r->df_inj_time / 1000.0) * r->rpm)/1200.0);

    return s;
}

double MdSensorRecord::getSpeed() const {
        return speed;
}
int MdSensorRecord::getGear() const {
        return gear;
}
int MdSensorRecord::getN75() const {
        return n75;
}
double MdSensorRecord::getN75ReqBoost() {
    return n75_req_boost;
}
quint8 MdSensorRecord::getN75ReqBoostPWM() {
    return n75_req_boost_pwm;
}
quint8 MdSensorRecord::getFlags() {
    return flags;
}

double MdSensorRecord::getVDOTemp1() const {
    return VDOTemp1;
}
double MdSensorRecord::getVDOTemp2() const {
	return VDOTemp2;
}
double MdSensorRecord::getVDOTemp3() const {
	return VDOTemp3;
}
double MdSensorRecord::getVDOPres1() const {
	return VDOPres1;
}
double MdSensorRecord::getVDOPres2() const {
	return VDOPres2;
}
double MdSensorRecord::getVDOPres3() const {
	return VDOPres3;
}

double MdSensorRecord::getEgt0() const {
    return egt[0];
}
double MdSensorRecord::getEgt1() const {
    return egt[1];
}
double MdSensorRecord::getEgt2() const {
    return egt[2];
}
double MdSensorRecord::getEgt3() const {
    return egt[3];
}
double MdSensorRecord::getEgt4() const {
    return egt[4];
}
double MdSensorRecord::getEgt5() const {
    return egt[5];
}
double MdSensorRecord::getEgt6() const {
    return egt[6];
}
double MdSensorRecord::getEgt7() const {
    return egt[7];
}

QMap<QString, double> MdSensorRecord::getHighestEgt() const {
    QMap<QString, double> r;
    double hv = -1;
    quint8 idx = 0;
    for ( int i = 0 ; i < AppEngine::getInstance()->numConnectedTypeK ; i++ ) {
        if ( egt[i] > hv ) {
            hv = egt[i];
            idx = i;
        }
    }
    r["temp"] = hv;
    r["idx"] = idx;
    return r;
}

double MdSensorRecord::getBatcur() const {
    return batcur;
}

double MdSensorRecord::getBoost() const {
    return boost;
}

double MdSensorRecord::getCasetemp() const {
    return casetemp;
}

double MdSensorRecord::getLambda() const {
    return lambda;
}

double MdSensorRecord::getLmm() const {
    return lmm;
}

int MdSensorRecord::getRpm() const {
    return rpm;
}

int MdSensorRecord:: getThrottle() const {
    return throttle;
}

int MdSensorRecord::getTime() const {
    return time;
}

void MdSensorRecord::setEgt0(double agt0) {
    this->egt[0] = agt0;
}

void MdSensorRecord::setEgt1(double agt1) {
    this->egt[1] = agt1;
}
void MdSensorRecord::setEgt2(double agt) {
    this->egt[2] = agt;
}
void MdSensorRecord::setEgt3(double agt) {
    this->egt[3] = agt;
}
void MdSensorRecord::setEgt4(double agt) {
    this->egt[4] = agt;
}
void MdSensorRecord::setEgt5(double agt) {
    this->egt[5] = agt;
}
void MdSensorRecord::setEgt6(double agt) {
    this->egt[6] = agt;
}
void MdSensorRecord::setEgt7(double agt) {
    this->egt[7] = agt;
}

void MdSensorRecord::setBatcur(double batcur) {
    this->batcur = batcur;
}

void MdSensorRecord::setBoost(double boost) {
    this->boost = boost;
}

void MdSensorRecord::setCasetemp(double casetemp) {
    this->casetemp = casetemp;
}

void MdSensorRecord::setLambda(double lambda) {
    this->lambda = lambda;
}

void MdSensorRecord::setLmm(double lmm) {
    this->lmm = lmm;
}

void MdSensorRecord::setRpm(int rpm) {
    this->rpm = rpm;
}

void MdSensorRecord::setThrottle(int throttle) {
    this->throttle = throttle;
}

void MdSensorRecord::setTime(int time) {
    this->time = time;
}

void MdSensorRecord::setSpeed(int s) {
    this->speed = s;
}
void MdSensorRecord::setGear(int g) {
    this->gear = g;
}
void MdSensorRecord::setN75(int n) {
    this->n75 = n;
}
void MdSensorRecord::setN75ReqBoost (double b) {
    this->n75_req_boost = b;
}
void MdSensorRecord::setN75ReqBoostPWM (quint8 p) {
    this->n75_req_boost_pwm = p;
}
void MdSensorRecord::setFlags (quint8 f) {
    this->flags = f;
}


QVariant MdSensorRecord::getColumn ( const int & column ) {
	switch ( column ) {
        case 0:
            return time;
        case 1:
            return rpm;
        case 2:
            return boost;
        case 3:
            return throttle;
        case 4:
            return lambda;
        case 5:
            return lmm;
        case 6:
            return casetemp;
        case 7:
            return egt[0];
        case 8:
            return egt[1];
        case 9:
            return egt[2];
        case 10:
            return egt[3];
        case 11:
            return egt[4];
        case 12:
            return egt[5];
        case 13:
            return egt[6];
        case 14:
            return egt[7];
        case 15:
            return batcur;
        case 16:
            return VDOPres1;
        case 17:
            return VDOPres2;
        case 18:
            return VDOPres3;
        case 19:
            return VDOTemp1;
        case 20:
            return VDOTemp2;
        case 21:
            return VDOTemp3;
        case 22:
            return speed;
        case 23:
            return gear;
        case 24:
            return n75;
        case 25:
            return n75_req_boost;
        case 26:
            return n75_req_boost_pwm;
        case 27:
            return efr_speed;
        case 28:
            return df_ignition;
        case 29:
            return df_ignition_total_retard;
        case 30:
            return df_inj_time;
        case 31:
            return df_iat;
        case 32:
            return df_ect;
        case 33:
            return df_knock_raw ;
        case 34:
        // makes 0-250kpa
        //    return (((5.0*(df_boost_raw/255.0))+0.2 ) / 0.02 ) ;
            return df_boost_raw;
        case 35:
            return df_cold_startup_enrichment;
        case 36:
            return df_warm_startup_enrichment;
        case 37:
            return df_ect_enrichment;
        case 38:
            return df_iat_enrichment;
        case 39:
            return df_counter_startup_enrichment;
        case 40:
            return df_voltage;
        case 41:
            return df_acceleration_enrichment;
        case 42:
            return df_ect_injection_addon;
        case 43:
            return df_lambda;
        case 44:
            return df_co_poti;
        case 45:
            return df_isv;
        case 46:
            return df_lc_flags;
        case 47:
            return QString::number(df_kline_freq) + " / " + QString::number(df_kline_framenum);
        //>=48 is data from smartphone in MobileSensorData

        }
	return QVariant();
}


MdDataRecord::MdDataRecord ( ) {
	sensorR = new MdSensorRecord();
    mobileR = new MobileSensorRecord();
}
MdDataRecord::MdDataRecord(MdSensorRecord *sr) : sensorR(sr)  {
    mobileR = new MobileSensorRecord();

#if defined ( Q_WS_MAEMO_5 )
    Accelerometer* a = AppEngine::getInstance()->getAccelerometer();
    MobileGPS* g = AppEngine::getInstance()->getGps();

    if ( a ) {
        mobileR->accX = a->x;
        mobileR->accY = a->y;
        mobileR->accZ = a->z;
    }

    if ( g ) {
        mobileR->mdTimestamp = sr->getTime();
        mobileR->gpsTimestamp = g->lastPos().timestamp();
        mobileR->gpsCoordinateString = g->lastPos().coordinate().toString();
        mobileR->gpsAltitude = g->lastPos().coordinate().altitude();
        mobileR->gpsGroundSpeed = g->lastPos().attribute(QGeoPositionInfo::GroundSpeed);
        mobileR->gpsDirection = g->lastPos().attribute(QGeoPositionInfo::Direction);
        mobileR->gpsHorizontalAccuracy = g->lastPos().attribute(QGeoPositionInfo::HorizontalAccuracy);
        mobileR->gpsVerticalAccuracy = g->lastPos().attribute(QGeoPositionInfo::VerticalAccuracy);
        mobileR->gpsUpdateCount = g->updateCount();
        mobileR->gpsValid = g->lastPos().isValid();
        mobileR->millisElapsedSinceLastMdFrame = g->elapsedSinceLastMdFrame;
    }
#endif
}

MdDataRecord::~MdDataRecord ( ) {
	if ( sensorR )
		delete (sensorR);
    if ( mobileR )
        delete (mobileR);
}

MdSensorRecord* MdDataRecord::getSensorR() const {
    return sensorR;
}

void MdDataRecord::setSensorR(MdSensorRecord *sensorR) {
    this->sensorR = sensorR;
}

QVariant MdDataRecord::getColumn ( const int & column ) {
    // colum count!
    if ( column >= 0 && column <= 47 ) {
        if ( sensorR )
            return sensorR->getColumn ( column );
    } else {
        if ( mobileR )
            return mobileR->getColumn (column);
    }
    return QVariant();
}


QDataStream& operator<< (QDataStream& s, MdDataRecord *d) {
	s << d->sensorR;
    s << d->mobileR;
	return s;
}
QDataStream& operator>> (QDataStream& s, MdDataRecord *d) {
	s >> d->sensorR;
    //hack to be able to import old saved files which stored empty MdBoostPidRecords!
//    MdBoostPidRecord *t = new MdBoostPidRecord ();
//    s >> t;
//    delete (t);
    s >> d->mobileR;
	return s;
}

