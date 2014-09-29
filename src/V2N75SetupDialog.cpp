#include "V2N75SetupDialog.h"
#include "QDebug"
#include <QThread>
#include <QMessageBox>
#include "com/MdBinaryProtocol.h"
#include "widgets/MyTableWidget.h"
#include "widgets/N75PidSettingsWidget.h"
#include "ui_V2N75SetupDialog.h"
#include <unistd.h>
#include <QtGlobal>
#include <AppEngine.h>

#ifdef Q_WS_MAEMO_5
    #include <QtMaemo5>
    #include <QMaemo5InformationBox>
#endif


V2N75SetupDialog::V2N75SetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::V2N75SetupDialog),
    t (new QTimer(this)),
    wt(new QTimer(this))
{
    ui->setupUi(this);
    n75lowTw = new N75TableWidget (0, ui->n75TableGroupBox);
    n75highTw = new N75TableWidget (1, ui->n75TableGroupBox);
    ui->n75TableGroupBox->layout()->addWidget(n75lowTw);
    ui->n75TableGroupBox->layout()->addWidget(n75highTw);
    ui->n75TableGroupBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );

    if ( ui->n75comboBox->currentIndex() == 0 ) {
        n75lowTw->show();
        n75highTw->hide();
    } else {
        n75lowTw->hide();
        n75highTw->show();
    }
    qDebug() << ui->n75comboBox->currentText();
    connect (ui->n75comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(n75modeChanged(int)) );
    connect (ui->n75WritePushButton, SIGNAL(clicked()), this, SLOT(n75writeSlow()));
    connect (ui->n75ReadPushButton, SIGNAL(clicked()), this, SLOT(n75readSlow()));

    connect (t, SIGNAL(timeout()), this, SLOT(timerUpdateRead()));
    t->setSingleShot(true);
    connect (wt, SIGNAL(timeout()), this, SLOT(timerUpdateWrite()));
    wt->setSingleShot(true);

    n75Settings = new N75PidSettingsWidget (this);
    ui->n75TableGroupBox->layout()->addWidget (n75Settings);
}


V2N75SetupDialog::~V2N75SetupDialog()
{
    delete ui;
    delete t;
}

void V2N75SetupDialog::n75modeChanged(int index) {
    if ( ui->n75comboBox->currentIndex() == 0 ) {
        n75lowTw->show();
        n75highTw->hide();
    } else {
        n75lowTw->hide();
        n75highTw->show();
    }
    qDebug() << "n75 " << ui->n75comboBox->currentText() << " activated";
}

void V2N75SetupDialog::n75readSlow() {
    next_setpoint_or_gear = 1;
    next_gear = 0;
    emit n75reqDutyMap( next_gear, ui->n75comboBox->currentIndex(), next_gear);
    wait_for_n75_map_frame = true;
    t->start(25);
}
void V2N75SetupDialog::timerUpdateRead () {
    if ( wait_for_n75_map_frame ) {
        //only proceed if we dont wait for a frame
        //TODO test me!
        t->start(25);
        return;
    }
    if ( next_gear < 6 ) {
        if ( next_setpoint_or_gear == 0 ) {
            emit n75reqDutyMap( next_gear, ui->n75comboBox->currentIndex(), next_gear);
            next_setpoint_or_gear = 1;
        } else {
            emit n75reqSetpointMap( next_gear, ui->n75comboBox->currentIndex(), next_gear);
            next_setpoint_or_gear = 0;
            next_gear++;
        }
        wait_for_n75_map_frame = true;
        t->start(25);
    } else {
        next_gear = 99;
    }
}

void V2N75SetupDialog::n75read() {
    for ( quint8 i = 0 ; i < 6 ; i++ ) {
        emit n75reqDutyMap( i, ui->n75comboBox->currentIndex(), i);
//        sleep (1);
        emit n75reqSetpointMap( i, ui->n75comboBox->currentIndex(), i);
//        sleep (1);
    }
}

void V2N75SetupDialog::n75writeSlow() {
    next_gear_write = 0;
    next_setpoint_or_gear_write = 0;
    write_wait_for_ack_serial = 0;
    wt->start(25);
}
void V2N75SetupDialog::timerUpdateWrite () {
    /* ack serial numer 100+gear! */
    Q_ASSERT (next_gear_write < 6);
    if ( write_wait_for_ack_serial >= 100 ) {
        wt->start(25);
        return;
    }
    N75TableWidget *tw = n75lowTw;
    if ( ui->n75comboBox->currentIndex() == 1 )
        tw = n75highTw;

    if ( next_setpoint_or_gear_write == 0 ) {
        //duty

        QVector<quint8>* dataDuty = new QVector<quint8>();
        for (quint8 i = 0 ; i < 16 ; i++)
            dataDuty->append( tw->item( next_gear_write*2, i)->data(Qt::DisplayRole).toInt() );

        qDebug() << "emit n75writeDutyMap gear=" << next_gear_write << " mode=" << ui->n75comboBox->currentIndex()
                 << " " << *dataDuty;

        write_wait_for_ack_serial = next_gear_write + 100;
        emit n75writeDutyMap ( next_gear_write, ui->n75comboBox->currentIndex(), write_wait_for_ack_serial, dataDuty);
        next_setpoint_or_gear_write = 1;
    } else {
        //setpoint
        QVector<double>* dataSp = new QVector<double>();
        for (quint8 i = 0 ; i < 16 ; i++) {
            Q_ASSERT ( next_gear_write*2 +1 <= tw->size().width() );
            Q_ASSERT ( i <= tw->size().height() );
            dataSp->append( tw->item( next_gear_write*2 + 1, i)->data(Qt::DisplayRole).toDouble() );
        }

        qDebug() << "emit n75writeSetpointMap gear=" << next_gear_write << " mode=" << ui->n75comboBox->currentIndex() <<
                " " << *dataSp;

        write_wait_for_ack_serial = next_gear_write + 100;
        emit n75writeSetpointMap ( next_gear_write, ui->n75comboBox->currentIndex(), write_wait_for_ack_serial, dataSp);
        next_setpoint_or_gear_write = 0;
        next_gear_write++;
    }
    if ( next_gear_write < 6 )
        wt->start(25);
    else {
        next_gear_write = 99;
#if  !defined (Q_WS_MAEMO_5)  && !defined (ANDROID)
        QMessageBox::information(this, "N75 maps", "write complete", QMessageBox::Ok);
#endif
#if  defined (Q_WS_MAEMO_5)
        QMaemo5InformationBox::information ( this, "write complete", 0 );
#endif
    }
}

void V2N75SetupDialog::n75write() {
    qDebug() << "n75write";

    N75TableWidget *tw = n75lowTw;
    if ( ui->n75comboBox->currentIndex() == 1 )
        tw = n75highTw;

    quint8 j = 0;
    for ( quint8 next_gear = 0 ; next_gear < 6 ; next_gear++ ) {
        QVector<quint8>* dataDuty = new QVector<quint8>();
        QVector<double>* dataSp = new QVector<double>();
        for (quint8 i = 0 ; i < 16 ; i++)
            dataDuty->append( tw->item( j, i)->data(Qt::DisplayRole).toInt() );
        emit n75writeDutyMap ( next_gear, ui->n75comboBox->currentIndex(), next_gear, dataDuty);
        qDebug() << "emit n75writeDutyMap " << next_gear << " " << ui->n75comboBox->currentIndex() <<
                " " << *dataDuty;
        j++;

        for (quint8 i = 0 ; i < 16 ; i++)
            dataSp->append( tw->item( j, i)->data(Qt::DisplayRole).toDouble() );
        emit n75writeSetpointMap ( next_gear, ui->n75comboBox->currentIndex(), next_gear, dataSp);
        qDebug() << "emit n75writeSetpointMap " << next_gear << " " << ui->n75comboBox->currentIndex() <<
                " " << *dataSp;
        j++;
//      sleep (1);
    }
}

void V2N75SetupDialog::n75dutyMap (quint8 gear, quint8 mode, quint8 serial, QVector<quint8> *data) {
    qDebug() << "[V2N75SetupDialog::n75dutyMap] gear=" << gear << " mode=" << mode << " serial=" << serial;
    Q_ASSERT (gear < 6);
    Q_ASSERT (wait_for_n75_map_frame == true);
    wait_for_n75_map_frame = false;
    if ( mode == 0 ) {
        //low
        for (quint8 i = 0 ; i < 16 ; i++)
            n75lowTw->item(gear*2, i) ->setText( QString::number( data->at(i) ) );
    } else {
        //high
        for (quint8 i = 0 ; i < 16 ; i++)
            n75highTw->item(gear*2, i) ->setText( QString::number ( data->at(i) ) );
    }
    delete data;
}
void V2N75SetupDialog::n75SetpointMap (quint8 gear, quint8 mode, quint8 serial, QVector<double> *data) {
    qDebug() << "n75SetpointMap gear=" << gear << " mode=" << mode << " serial=" << serial;
    Q_ASSERT (gear < 6);
    Q_ASSERT (wait_for_n75_map_frame == true);
    wait_for_n75_map_frame = false;
    if ( mode == 0 ) {
        //low
        for (quint8 i = 0 ; i < 16 ; i++)
            n75lowTw->item(gear*2 + 1, i) ->setText( QString::number( data->at(i) ) );
    } else {
        //high
        for (quint8 i = 0 ; i < 16 ; i++)
            n75highTw->item(gear*2 +1, i) ->setText( QString::number ( data->at(i) ) );
    }
    delete data;
}

void V2N75SetupDialog::showEvent ( QShowEvent * event ) {
    wait_for_n75_map_frame = false;
    write_wait_for_ack_serial = 0;
#if  defined (Q_WS_MAEMO_5)  || defined (ANDROID)
    dashboardActualizeSave = AppEngine::getInstance()->getActualizeDashboard() ;
    vis1ActualizeSave = AppEngine::getInstance()->getActualizeVis1();
    AppEngine::getInstance()->setActualizeDashboard( false );
    AppEngine::getInstance()->setActualizeVis1( false );
#endif
}

void V2N75SetupDialog::closeEvent ( QCloseEvent * event ) {
#if  defined (Q_WS_MAEMO_5)  || defined (ANDROID)
    AppEngine::getInstance()->setActualizeDashboard( dashboardActualizeSave );
    AppEngine::getInstance()->setActualizeVis1( vis1ActualizeSave );
#endif
}

void V2N75SetupDialog::ackReceived (quint8 serial) {
    qDebug() << "V2N75SetupDialog::ackReceived serial " << serial;
    if ( serial == write_wait_for_ack_serial ) {
        qDebug() << "ack for write received " << serial;
        write_wait_for_ack_serial = 0;
    }
}
