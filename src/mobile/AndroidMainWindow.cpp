#include <QCloseEvent>
#include <QtCore/qmath.h>
#include <QDebug>
#include <QLabel>

#include "AndroidMainWindow.h"
#include "ui_AndroidMainWindow.h"
#include "AppEngine.h"

#if defined(Q_OS_ANDROID)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    #include <QAndroidJniObject>
    #include <QAndroidJniEnvironment>
    #include <QtAndroid>
#else
    #include <QCoreApplication>
    #include <QtCore/private/qandroidextras_p.h>
#endif
    //FIX hack! fix for "java.lang.UnsatisfiedLinkError: dlopen failed: library libQt5Concurrent_x86.so libQt5PrintSupport_x86.so not found.
    #include <QtPrintSupport>
    #include <QtConcurrent>
#endif


AndroidMainWindow::AndroidMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AndroidMainWindow)
{
    ui->setupUi(this);


     timer.start();

     grabGesture(Qt::TapGesture);
     grabGesture(Qt::TapAndHoldGesture);
     grabGesture(Qt::PanGesture);
     grabGesture(Qt::PinchGesture);
     grabGesture(Qt::SwipeGesture);

     //test: moved to AndroidDashboardDialog
     /*
     connect (ui->actionMdSupportForum, SIGNAL(triggered()), this, SLOT(fireSupportForumIntent()) );
     connect (ui->fbPushButton, SIGNAL(clicked()), this, SLOT(fireFbPageIntent()) );
    */
     QSettings settings;
     if ( settings.value("md/md", QVariant ( MDMODE ).toBool() )  == false ) {
         ui->actionV2_N75_Settings->setVisible(false);
         ui->actionGearbox_settings->setVisible(false);

         ui->menuData->setTitle("Digifant 1");
         ui->actionMdSupportForum->setText("more information");
     }

#if defined (DIGIFANTVANAPP)
     ui->actionV2_N75_Settings->setVisible(false);
     ui->actionGearbox_settings->setVisible(false);
     ui->menuData->setTitle("Digifant 1");
     ui->actionMdSupportForum->setText("more information");
#endif

#if not defined (Q_OS_IOS)
     _pixmapBg.load(":/mm/img/dfo_schwarz.png");
#endif
    
#if defined (Q_OS_IOS)
    ui->actionMdSupportForum->setVisible(false);
    statusBarLabel = new QLabel(ui->statusbar);
    ui->statusbar->addWidget(statusBarLabel);
    statusBarLabel->setText(tr("BT not connected"));
    //ui->statusbar->showMessage ("G60 digifantview for IOS build on " + BUILDDATETIME);
    ui->statusbar->showMessage ("G60 / IOS", 3000);
#endif
#if defined (Q_OS_ANDROID)
    statusBarLabel = new QLabel(ui->statusbar);
    ui->statusbar->addWidget(statusBarLabel);
    statusBarLabel->setText(tr("BT not connected"));
#endif
}

AndroidMainWindow::~AndroidMainWindow()
{
    delete ui;
}

bool AndroidMainWindow::event(QEvent *event)
{
    //qDebug() << "AndroidMainWindow::event event type=" << event->type();
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));
    if (event->type() == QEvent::ChildRemoved) {
        QChildEvent* ce = dynamic_cast<QChildEvent*>(event);
        if ( ce->child()->isWidgetType() )
            AppEngine::getInstance()->reCreateDialogsAndroidFix();
    }
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
    AppEngine::getInstance()->getMdCom()->closePort();
    emit writeSettings();
    event->accept();
    QMainWindow::closeEvent ( event );
}

void AndroidMainWindow::resizeEvent(QResizeEvent *event)
{
    qDebug() << "AndroidMainWindow::resizeEvent size=" << event->size() << " old=" << event->oldSize();
    //ui->fbPushButton->setMinimumHeight( ui->dashboardPushButton->height() );
    QMainWindow::resizeEvent(event);
}

//void AndroidMainWindow::paintEvent(QPaintEvent *pe)
//{
//    QPainter painter(this);
//    int widWidth = this->ui->centralwidget ->width();
//    int widHeight = this->ui->centralwidget->height();
//    _pixmapBg = _pixmapBg.scaled(widWidth, widHeight, Qt::KeepAspectRatioByExpanding);
//    painter.drawPixmap(0, 0, _pixmapBg);

////    auto winSize = size();
////    auto pixmapRatio = (float)_pixmapBg.width() / _pixmapBg.height();
////    auto windowRatio = (float)winSize.width() / winSize.height();

////    if(pixmapRatio > windowRatio) {
////        auto newWidth = (int)(winSize.height() * pixmapRatio);
////        auto offset = (newWidth - winSize.width()) / -2;
////        painter.drawPixmap(offset, 0, newWidth, winSize.height(), _pixmapBg);
////    } else {
////        auto newHeight = (int)(winSize.width() / pixmapRatio);
////        auto offset = (newHeight - winSize.height()) / -2;
////        painter.drawPixmap(0, offset, winSize.width(), newHeight, _pixmapBg);
////    }
//}

#if defined Q_OS_ANDROID
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool AndroidMainWindow::checkAndroidPermission(QString permission)
{
    auto result = QtAndroidPrivate::checkPermission(permission).then([](
        QtAndroidPrivate::PermissionResult result) {
            return result;
        });
    result.waitForFinished();

    return result.result() != QtAndroidPrivate::PermissionResult::Denied;
}

bool AndroidMainWindow::requestAndroidPermission(QString permission)
{
    auto result = QtAndroidPrivate::requestPermission(permission).then([](
        QtAndroidPrivate::PermissionResult result) {
            return result;
        });
    result.waitForFinished();
    return result.result() != QtAndroidPrivate::PermissionResult::Denied;
}
#endif
#endif

void AndroidMainWindow::showStatusMessage(const QString &msg)
{
    qDebug() << "showStatusMessage" << msg;
    //test: moved to AndroidDashboardDialog
    /*
    if ( ui->textEdit )
        ui->textEdit->insertPlainText( msg + "\n" );
    */
#if defined(Q_OS_ANDROID)
    showToast(msg);
    if ( msg.contains("connected") ) {
        statusBarLabel->setText(tr("BT connected"));
    }
    if ( msg.contains("disconnected") ) {
        statusBarLabel->setText(tr("BT not connected"));
    //qDebug() << "STATUS " << msg;
    }
#endif
#if defined(Q_OS_IOS)
    ui->statusbar->showMessage(msg, 4000);
    if ( msg.contains("connected") ) {
        statusBarLabel->setText(tr("BT connected"));
    }
    if ( msg.contains("disconnected") ) {
        statusBarLabel->setText(tr("BT not connected"));
    }
    //qDebug() << "STATUS " << msg;
#endif
}

void AndroidMainWindow::showToast(const QString &message, ToastDuration duration)
{
#if defined(Q_OS_ANDROID)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // all the magic must happen on Android UI thread
    QtAndroid::runOnAndroidThread([message, duration] {
        QAndroidJniObject javaString = QAndroidJniObject::fromString(message);
        QAndroidJniObject toast = QAndroidJniObject::callStaticObjectMethod("android/widget/Toast", "makeText",
                                                                            "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;",
                                                                            QtAndroid::androidActivity().object(),
                                                                            javaString.object(),
                                                                            jint(duration));
        toast.callMethod<void>("show");
    });
#else
    // Qt 6 TODO TEST
    // all the magic must happen on Android UI thread
    // https://bugreports.qt.io/browse/QTBUG-90501
     QNativeInterface::QAndroidApplication::runOnAndroidMainThread([message, duration] {
        QJniObject javaString = QJniObject::fromString(message);
        QJniObject toast = QJniObject::callStaticObjectMethod("android/widget/Toast", "makeText",
                                                                            "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;",
                                                                            QNativeInterface::QAndroidApplication::context(), //QtAndroid::androidActivity().object(),
                                                                            javaString.object(),
                                                                            jint(duration));
        toast.callMethod<void>("show");
    });
#endif
#else
    qDebug() << "toasts are availabe in androiy only!";
#endif
}

void AndroidMainWindow::btPortClosed()
{
    ui->actionBluetoothToggleState->setText("Bluetooth connect");
    qDebug() << "BT port closed";
}

void AndroidMainWindow::btPortOpened()
{
    ui->actionBluetoothToggleState->setText("Bluetooth disconnect");
    qDebug() << "BT port opened";
}

void AndroidMainWindow::fireSupportForumIntent()
{
#ifdef Q_OS_ANDROID
//    qDebug() << "fireSupportForumIntent()";
    QSettings settings;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QAndroidJniObject s1 = QAndroidJniObject::fromString("http://mdforum.designer2k2.at/viewtopic.php?f=15&t=334");
    if ( settings.value("md/md", QVariant ( MDMODE ).toBool() )  == false ) {
        s1 = QAndroidJniObject::fromString("http://digifant-einzelabstimmung.de/bofh-ng/de/digifant-1/live-daten-auslesen");
    }
#else
     QJniObject s1 = QJniObject::fromString("http://mdforum.designer2k2.at/viewtopic.php?f=15&t=334");
     if ( settings.value("md/md", QVariant ( true ).toBool() )  == false ) {
         s1 = QJniObject::fromString("http://digifant-einzelabstimmung.de/bofh-ng/de/digifant-1/live-daten-auslesen");
     }
#endif



#if defined ( DIGIFANTAPP ) or defined ( DIGIFANTVANAPP )
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QAndroidJniObject::callStaticMethod<void>( "de/gummelinformatics/digifant/MuiIntentHelper",
                                           "openUrl",
                                           "(Ljava/lang/String;)V",
                                            s1.object<jstring>() );
#else
     QJniObject::callStaticMethod<void>( "de/gummelinformatics/digifant/MuiIntentHelper",
                                         "openUrl",
                                         "(Ljava/lang/String;)V",
                                         s1.object<jstring>() );
#endif
#else
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QAndroidJniObject::callStaticMethod<void>( "de/gummelinformatics/mui/MuiIntentHelper",
                                           "openUrl",
                                           "(Ljava/lang/String;)V",
                                            s1.object<jstring>() );
#else
     QJniObject::callStaticMethod<void>( "de/gummelinformatics/mui/MuiIntentHelper",
                                            "openUrl",
                                            "(Ljava/lang/String;)V",
                                             s1.object<jstring>() );
#endif
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
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
#else
    QJniEnvironment env;
    if (env->ExceptionCheck()) {
        // Handle exception here.
        qDebug() << "*** JNI exception ***";
        env->ExceptionDescribe();
        env->ExceptionClear();
        env->ExceptionClear();
    } else {
        qDebug() << "NO JNI exception";
    }
#endif

//    QDesktopServices::openUrl( QUrl("http://mdforum.designer2k2.at/") );

#endif
}


