#include <QCloseEvent>
#include <QtCore/qmath.h>
#include <QDebug>

#include "AndroidMainWindow.h"
#include "ui_AndroidMainWindow.h"
#include "AppEngine.h"

#if defined(Q_OS_ANDROID)
    #include <QAndroidJniObject>
    #include <QAndroidJniEnvironment>
#endif


AndroidMainWindow::AndroidMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AndroidMainWindow)
{
    ui->setupUi(this);


     t = QTime::currentTime();
     t.start();

     grabGesture(Qt::TapGesture);
     grabGesture(Qt::TapAndHoldGesture);
     grabGesture(Qt::PanGesture);
     grabGesture(Qt::PinchGesture);
     grabGesture(Qt::SwipeGesture);

     connect (ui->actionMdSupportForum, SIGNAL(triggered()), this, SLOT(fireSupportForumIntent()) );

     QSettings settings;
     if ( settings.value("md/md", QVariant ( MDMODE ).toBool() )  == false ) {
         ui->actionV2_N75_Settings->setVisible(false);
         ui->actionGearbox_settings->setVisible(false);

         ui->menuData->setTitle("Digifant 1");
         ui->actionMdSupportForum->setText("more information");
     }
}

AndroidMainWindow::~AndroidMainWindow()
{
    delete ui;
}

bool AndroidMainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));
    return QWidget::event(event);
}

bool AndroidMainWindow::gestureEvent(QGestureEvent *event)
{
    qDebug() << "gestureEvent():" << event->gestures().size();
    if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
//        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
        qDebug() << "swipe";
    else if (QGesture *pan = event->gesture(Qt::PanGesture))
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

void AndroidMainWindow::closeEvent(QCloseEvent *event) {
    //segfaults :(
    emit writeSettings();
    event->accept();
    QMainWindow::closeEvent ( event );
}

void AndroidMainWindow::resizeEvent(QResizeEvent *event)
{
    qDebug() << "AndroidMainWindow::resizeEvent size=" << event->size() << " old=" << event->oldSize();
    QMainWindow::resizeEvent(event);
}

void AndroidMainWindow::showStatusMessage(const QString &msg)
{
    qDebug() << "showStatusMessage";
    if ( ui->textEdit )
        ui->textEdit->insertPlainText( msg + "\n" );
}

void AndroidMainWindow::btPortClosed()
{
    ui->actionBluetoothToggleState->setText("Bluetooth connect");
}

void AndroidMainWindow::btPortOpened()
{
    ui->actionBluetoothToggleState->setText("Bluetooth disconnect");
}

void AndroidMainWindow::fireSupportForumIntent()
{
#ifdef Q_OS_ANDROID
//    qDebug() << "fireSupportForumIntent()";
    QAndroidJniObject s1 = QAndroidJniObject::fromString("http://mdforum.designer2k2.at/viewtopic.php?f=15&t=334");


    QAndroidJniObject::callStaticMethod<void>( "de/gummelinformatics/mui/MuiIntentHelper",
                                           "openUrl",
                                           "(Ljava/lang/String;)V",
                                            s1.object<jstring>() );

//    jboolean r = QAndroidJniObject::callStaticMethod<jboolean>( "de/gummelinformatics/mui/MuiIntentHelper",
//                                           "test");
//    if ( r )
//        qDebug() << "fireSupportForumIntent() debug TRUE";
//    else
//        qDebug() << "fireSupportForumIntent() debug FALSE";

    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        // Handle exception here.
        qDebug() << "*** JNI exception ***";
        env->ExceptionDescribe();
        env->ExceptionClear();
        env->ExceptionClear();
    } else {
        qDebug() << "NO JNI exception";
    }

//    QDesktopServices::openUrl( QUrl("http://mdforum.designer2k2.at/") );

#endif
}


