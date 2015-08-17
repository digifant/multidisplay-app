#include <QDebug>
#include <QGesture>
#include <QGestureEvent>
#include <QSwipeGesture>
#include <QMessageBox>
#include <QTimer>
#include <QVBoxLayout>
#include <mobile/SwipeGestureRecognizer.h>

#include <AppEngine.h>
#include "AndroidN75Dialog.h"
#include <widgets/MyTableWidget.h>
#include <widgets/N75PidSettingsWidget.h>
#include "ui_AndroidN75Dialog.h"

AndroidN75Dialog::AndroidN75Dialog(QWidget *parent, MdBinaryProtocol* mds ) : mds(mds), landscape(false),
    QDialog(parent),
    ui(new Ui::AndroidN75Dialog),
    t (new QTimer(this)),
    wt(new QTimer(this))
{
    ui->setupUi(this);
    Q_ASSERT(mds != NULL);

    QVBoxLayout *vl = new QVBoxLayout();
    vl->setContentsMargins(0,0,0,0);
    vl->setSpacing(0);
    ui->lowTab->setLayout(vl);

    QHBoxLayout *ll = new QHBoxLayout();
    ll->setContentsMargins(0,0,0,0);
    ll->setSpacing(0);
    vl->addLayout(ll);
    n75lowTw = new N75TableWidget (0, ui->lowTab);
    n75lowTw->setMinimumSize(400, 600);
    n75lowTw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ll->addWidget(n75lowTw);

    QHBoxLayout *lbl = new QHBoxLayout();
    lbl->setContentsMargins(0,0,0,0);
    lbl->setSpacing(0);
    QPushButton *lrb = new QPushButton ("read", ui->lowTab);
    lbl->addWidget(lrb);
    QPushButton *lwb = new QPushButton ("write", ui->lowTab);
    lbl->addWidget(lwb);
    QPushButton *lerb = new QPushButton ("EEread", ui->lowTab);
    lbl->addWidget(lerb);
    QPushButton *lewb = new QPushButton ("EEwrite", ui->lowTab);
    lbl->addWidget(lewb);
    vl->addLayout(lbl);

    QVBoxLayout *vh = new QVBoxLayout();
    vh->setContentsMargins(0,0,0,0);
    vh->setSpacing(0);
    ui->highTab->setLayout(vh);

    QHBoxLayout *hl = new QHBoxLayout();
    hl->setContentsMargins(0,0,0,0);
    hl->setSpacing(0);
    vh->addLayout (hl);
    n75highTw = new N75TableWidget (1, ui->highTab);
    n75highTw->setMinimumSize(400, 600);
    n75highTw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    hl->addWidget(n75highTw);

    QHBoxLayout *hbl = new QHBoxLayout();
    hbl->setContentsMargins(0,0,0,0);
    hbl->setSpacing(0);
    QPushButton *hrb = new QPushButton ("read", ui->lowTab);
    hbl->addWidget(hrb);
    QPushButton *hwb = new QPushButton ("write", ui->lowTab);
    hbl->addWidget(hwb);
    QPushButton *herb = new QPushButton ("EEread", ui->lowTab);
    hbl->addWidget(herb);
    QPushButton *hewb = new QPushButton ("EEwrite", ui->lowTab);
    hbl->addWidget(hewb);
    vh->addLayout(hbl);


    connect (t, SIGNAL(timeout()), this, SLOT(timerUpdateRead()));
    t->setSingleShot(true);
    connect (wt, SIGNAL(timeout()), this, SLOT(timerUpdateWrite()));
    wt->setSingleShot(true);

    QHBoxLayout *pl = new QHBoxLayout();
    pl->setContentsMargins(0,0,0,0);
    pl->setSpacing(0);
    ui->pidTab->setLayout(pl);
    n75Settings = new N75PidSettingsWidget (ui->pidTab, mds, false );
    pl->addWidget (n75Settings);

//    grabGesture(Qt::TapAndHoldGesture);
//    grabGesture(Qt::SwipeGesture);

    // Create a SWIPE recognizer because the default SWIPE recognizer
       // does not really work on Symbian device.
    QGestureRecognizer* pRecognizer = new SwipeGestureRecognizer();
    m_gestureId = QGestureRecognizer::registerRecognizer(pRecognizer);


    if ( mds ) {
        connect (this, SIGNAL(n75reqDutyMap(quint8,quint8,quint8)), mds, SLOT(mdCmdReqN75DutyMap(quint8,quint8,quint8)));
        connect (this, SIGNAL(n75reqSetpointMap(quint8,quint8,quint8)), mds, SLOT(mdCmdReqN75SetpointMap(quint8,quint8,quint8)));
        connect (mds, SIGNAL( n75DutyMapreceived (quint8, quint8, quint8, QVector<quint8>*)), this, SLOT(n75dutyMap(quint8,quint8,quint8,QVector<quint8>*)));
        connect (mds, SIGNAL( n75SetpointMapreceived (quint8, quint8, quint8, QVector<double>*)), this, SLOT(n75SetpointMap(quint8,quint8,quint8,QVector<double>*)));
        connect (this, SIGNAL(n75writeDutyMap(quint8,quint8,quint8,QVector<quint8>*)), mds, SLOT(mdCmdWriteN75DutyMap(quint8,quint8,quint8,QVector<quint8>*)));
        connect (this, SIGNAL(n75writeSetpointMap(quint8,quint8,quint8,QVector<double>*)), mds, SLOT(mdCmdWriteN75SetpointMap(quint8,quint8,quint8,QVector<double>*)));

        connect (lerb, SIGNAL(clicked()), mds, SLOT(mdCmdLoadN75MapsFromEEprom()));
        connect (lewb, SIGNAL(clicked()), mds, SLOT(mdCmdWriteN75MapsToEEprom()));
        connect (herb, SIGNAL(clicked()), mds, SLOT(mdCmdLoadN75MapsFromEEprom()));
        connect (hewb, SIGNAL(clicked()), mds, SLOT(mdCmdWriteN75MapsToEEprom()));
        connect (lrb, SIGNAL(clicked()), this, SLOT(n75readSlow()) );
        connect (hrb, SIGNAL(clicked()), this, SLOT(n75readSlow()) );
        connect (lwb, SIGNAL(clicked()), this, SLOT(n75writeSlow()) );
        connect (hwb, SIGNAL(clicked()), this, SLOT(n75writeSlow()) );

        connect (mds, SIGNAL(ackReceived (quint8)), this, SLOT(ackReceived(quint8)));

        //n75 pid connections are handled in its own class!
    }

}

AndroidN75Dialog::~AndroidN75Dialog()
{
    delete ui;
}


bool AndroidN75Dialog::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));
    return QWidget::event(event);
}

bool AndroidN75Dialog::swipeTriggered(QSwipeGesture *pSwipe) {
    bool result = false;

    if (pSwipe->state() == Qt::GestureFinished) {
        qDebug("Swipe angle: %f", pSwipe->swipeAngle());
        switch (SwipeGestureUtil::GetHorizontalDirection(pSwipe)) {
            case QSwipeGesture::Left:
            {
                qDebug("Swipe Left detected");

                int i = ui->tabWidget->currentIndex() -1 ;
                if ( i < 0 )
                    i = ui->tabWidget->count() - 1;
                ui->tabWidget->setCurrentIndex( i );

                result = true;

                break;
            }
            case QSwipeGesture::Right:
            {
                qDebug("Swipe Right detected");
                int i = ui->tabWidget->currentIndex() +1 ;
                if ( i >= ui->tabWidget->count() )
                    i = 0;
                ui->tabWidget->setCurrentIndex( i );

                result = true;

                break;
            }
            default:
                break;
        }
        switch (SwipeGestureUtil::GetVerticalDirection(pSwipe)) {
            case QSwipeGesture::Up:
                qDebug("Swipe Up detected");
                result = true;
                break;
            case QSwipeGesture::Down:
                qDebug("Swipe Down detected");
                result = true;
                break;
            default:
                break;
        }
    }
    return result;
}

bool AndroidN75Dialog::gestureEvent(QGestureEvent *event) {

    qDebug() << "gestureEvent():" << event->gestures().size();
    if (QGesture *swipe = event->gesture(Qt::SwipeGesture)) {
        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
        qDebug() << "swipe";
    } else if (QGesture *pan = event->gesture(Qt::PanGesture))
//        panTriggered(static_cast<QPanGesture *>(pan));
        qDebug() << "pan";
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
//        pinchTriggered(static_cast<QPinchGesture *>(pinch));
        qDebug() << "pinch";
    if (QGesture *tap = event->gesture(Qt::TapGesture))
//        pinchTriggered(static_cast<QPinchGesture *>(pinch));
        qDebug() << "tap";
    else if (QGesture *tapAndHold = event->gesture(Qt::TapAndHoldGesture))
    //        pinchTriggered(static_cast<QPinchGesture *>(pinch));
        qDebug() << "tap and hold";
    return true;
}


void AndroidN75Dialog::resizeEvent ( QResizeEvent * event ) {
    qDebug() << "AndroidN75Dialog::resizeEvent width=" << event->size().width() << " height=" << event->size().height();
    if ( event ) {
//        recalcDataFontSize = true;
//        dataFont.setPointSize( (event->size().width()) / digits );

        if ( event->size().height() > 1.5 * event->size().width()  ){
            landscape = false;
            ui->tabWidget->setTabPosition(QTabWidget::North);
        } else {
            landscape = true;
            //bugfix: breaks the swipe recognizer!
//            ui->tabWidget->setTabPosition(QTabWidget::West);
        }
    }
}



void AndroidN75Dialog::n75readSlow(int idx) {
    if (( idx == 0 ) || (idx == 1 ))
            n75_idx = idx;
    else {
        int i = ui->tabWidget->currentIndex();
        if (( i == 0 ) || (i == 1 ))
            n75_idx = i;
        else {
            QMessageBox::warning(this, "N75 map idx", "wrong idx " + i, QMessageBox::Ok);
            return;
        }
    }
    next_setpoint_or_gear = 1;
    next_gear = 0;
    emit n75reqDutyMap( next_gear, n75_idx, next_gear);
    wait_for_n75_map_frame = true;
    t->start(25);
}
void AndroidN75Dialog::timerUpdateRead () {
    if ( wait_for_n75_map_frame ) {
        //only proceed if we dont wait for a frame
        //TODO test me!
        t->start(25);
        return;
    }
    if ( next_gear < 6 ) {
        if ( next_setpoint_or_gear == 0 ) {
            emit n75reqDutyMap( next_gear, n75_idx, next_gear);
            next_setpoint_or_gear = 1;
        } else {
            emit n75reqSetpointMap( next_gear, n75_idx, next_gear);
            next_setpoint_or_gear = 0;
            next_gear++;
        }
        wait_for_n75_map_frame = true;
        t->start(25);
    } else {
        next_gear = 99;
    }
}

//void AndroidN75Dialog::n75read() {
//    for ( quint8 i = 0 ; i < 6 ; i++ ) {
//        emit n75reqDutyMap( i, ui->n75comboBox->currentIndex(), i);
////        sleep (1);
//        emit n75reqSetpointMap( i, ui->n75comboBox->currentIndex(), i);
////        sleep (1);
//    }
//}

void AndroidN75Dialog::n75writeSlow(int idx) {
    if (( idx == 0 ) || (idx == 1 ))
            n75_idx = idx;
    else {
        int i = ui->tabWidget->currentIndex();
        if (( i == 0 ) || (i == 1 ))
            n75_idx = i;
        else {
            QMessageBox::warning(this, "N75 map idx", "wrong idx " + i, QMessageBox::Ok);
            return;
        }
    }
    next_gear_write = 0;
    next_setpoint_or_gear_write = 0;
    write_wait_for_ack_serial = 0;
    wt->start(25);
}
void AndroidN75Dialog::timerUpdateWrite () {
    /* ack serial numer 100+gear! */
    Q_ASSERT (next_gear_write < 6);
    if ( write_wait_for_ack_serial >= 100 ) {
        wt->start(25);
        return;
    }
    N75TableWidget *tw = n75lowTw;
    if ( n75_idx == 1 )
        tw = n75highTw;

    if ( next_setpoint_or_gear_write == 0 ) {
        //duty

        QVector<quint8>* dataDuty = new QVector<quint8>();
        for (quint8 i = 0 ; i < 16 ; i++)
            dataDuty->append( tw->item( next_gear_write*2, i)->data(Qt::DisplayRole).toInt() );

        qDebug() << "emit n75writeDutyMap gear=" << next_gear_write << " mode=" << n75_idx
                 << " " << *dataDuty;

        write_wait_for_ack_serial = next_gear_write + 100;
        emit n75writeDutyMap ( next_gear_write, n75_idx, write_wait_for_ack_serial, dataDuty);
        next_setpoint_or_gear_write = 1;
    } else {
        //setpoint
        QVector<double>* dataSp = new QVector<double>();
        for (quint8 i = 0 ; i < 16 ; i++) {
            Q_ASSERT ( next_gear_write*2 +1 <= tw->size().width() );
            Q_ASSERT ( i <= tw->size().height() );
            dataSp->append( tw->item( next_gear_write*2 + 1, i)->data(Qt::DisplayRole).toDouble() );
        }

        qDebug() << "emit n75writeSetpointMap gear=" << next_gear_write << " mode=" << n75_idx <<
                " " << *dataSp;

        write_wait_for_ack_serial = next_gear_write + 100;
        emit n75writeSetpointMap ( next_gear_write, n75_idx, write_wait_for_ack_serial, dataSp);
        next_setpoint_or_gear_write = 0;
        next_gear_write++;
    }
    if ( next_gear_write < 6 )
        wt->start(25);
    else {
        next_gear_write = 99;
#if  !defined (Q_WS_MAEMO_5)
        QMessageBox::information(this, "N75 maps", "write complete", QMessageBox::Ok);
#endif
#if  defined (Q_WS_MAEMO_5)
//        QMaemo5InformationBox::information ( this, "write complete", 0 );
        QMessageBox::information(this, "N75 maps", "write complete", QMessageBox::Ok);
#endif
    }
}

//void AndroidN75Dialog::n75write() {
//    qDebug() << "n75write";

//    N75TableWidget *tw = n75lowTw;
//    if ( ui->n75comboBox->currentIndex() == 1 )
//        tw = n75highTw;

//    quint8 j = 0;
//    for ( quint8 next_gear = 0 ; next_gear < 6 ; next_gear++ ) {
//        QVector<quint8>* dataDuty = new QVector<quint8>();
//        QVector<double>* dataSp = new QVector<double>();
//        for (quint8 i = 0 ; i < 16 ; i++)
//            dataDuty->append( tw->item( j, i)->data(Qt::DisplayRole).toInt() );
//        emit n75writeDutyMap ( next_gear, ui->n75comboBox->currentIndex(), next_gear, dataDuty);
//        qDebug() << "emit n75writeDutyMap " << next_gear << " " << ui->n75comboBox->currentIndex() <<
//                " " << *dataDuty;
//        j++;

//        for (quint8 i = 0 ; i < 16 ; i++)
//            dataSp->append( tw->item( j, i)->data(Qt::DisplayRole).toDouble() );
//        emit n75writeSetpointMap ( next_gear, ui->n75comboBox->currentIndex(), next_gear, dataSp);
//        qDebug() << "emit n75writeSetpointMap " << next_gear << " " << ui->n75comboBox->currentIndex() <<
//                " " << *dataSp;
//        j++;
////      sleep (1);
//    }
//}

void AndroidN75Dialog::n75dutyMap (quint8 gear, quint8 mode, quint8 serial, QVector<quint8> *data) {
    qDebug() << "[AndroidN75Dialog::n75dutyMap] gear=" << gear << " mode=" << mode << " serial=" << serial;
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
void AndroidN75Dialog::n75SetpointMap (quint8 gear, quint8 mode, quint8 serial, QVector<double> *data) {
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

void AndroidN75Dialog::showEvent ( QShowEvent * event ) {
    wait_for_n75_map_frame = false;
    write_wait_for_ack_serial = 0;
//#if  defined (Q_WS_MAEMO_5)  || defined (ANDROID)
//    dashboardActualizeSave = AppEngine::getInstance()->getActualizeDashboard() ;
//    vis1ActualizeSave = AppEngine::getInstance()->getActualizeVis1();
//    AppEngine::getInstance()->setActualizeDashboard( false );
//    AppEngine::getInstance()->setActualizeVis1( false );
//#endif
}

void AndroidN75Dialog::closeEvent ( QCloseEvent * event ) {
//#if  defined (Q_WS_MAEMO_5)  || defined (ANDROID)
//    AppEngine::getInstance()->setActualizeDashboard( dashboardActualizeSave );
//    AppEngine::getInstance()->setActualizeVis1( vis1ActualizeSave );
//#endif
}

void AndroidN75Dialog::ackReceived (quint8 serial) {
    qDebug() << "AndroidN75Dialog::ackReceived serial " << serial;
    if ( serial == write_wait_for_ack_serial ) {
        qDebug() << "ack for write received " << serial;
        write_wait_for_ack_serial = 0;
    }
}
