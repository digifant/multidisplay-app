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

#include "MobileVis1MainWindow.h"
#include "ui_MobileVis1MainWindow.h"
//#include "N900SwipeGesture.h"

#include<QStatusBar>
#include<QDebug>

MobileVis1MainWindow::MobileVis1MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MobileVis1MainWindow)
{
    ui->setupUi(this);
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow);
#endif

#if  defined (Q_WS_X11)  && !defined (Q_WS_MAEMO5)
    qDebug()  << "Q_WS_X11";
    //http://standards.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
    //http://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
    ui->ReplayStopButton->setIcon(  QIcon::fromTheme ("media-playback-start") );
    ui->ReplayPauseButton->setIcon(  QIcon::fromTheme ("media-playback-pause") );
    ui->ReplayStopButton->setIcon(  QIcon::fromTheme ("media-playback-stop") );

    ui->DVMicroLeftButton->setIcon( QIcon::fromTheme("media-seek-backward") );
    ui->DVLeftButton->setIcon( QIcon::fromTheme("media-skip-backward") );
#endif

    QStatusBar* sb = statusBar();
    if ( sb )
        sb->hide();

//    grabGesture(Qt::PanGesture);
//    grabGesture(Qt::PinchGesture);
//    grabGesture(Qt::SwipeGesture);


//    swipeGesture = new N900SwipeGesture(this);
//    connect(swipeGesture,SIGNAL(handleSwipe(N900SwipeGesture::SwipeDirection)),
//            this,SLOT(swipe(N900SwipeGesture::SwipeDirection)));
}

MobileVis1MainWindow::~MobileVis1MainWindow()
{
    delete ui;
}

void MobileVis1MainWindow::enableReplay() {
    ui->ReplayFrame->show();
}

void MobileVis1MainWindow::disableReplay() {
    ui->ReplayFrame->hide();
}

//bool MobileVis1MainWindow::event(QEvent *event)
//{
//    swipeGesture->handleEvent(event);
//    return QWidget::event(event);
//}
//void MobileVis1MainWindow::swipe(N900SwipeGesture::SwipeDirection direction)
//{
//    qDebug() << "swipe" << direction;
//}

/* broken on the N900... thanks Nokia! */
//bool MobileVis1MainWindow::event(QEvent *event) {
//    if (event->type() == QEvent::Gesture) {
//        qDebug() << "event(QEvent *event) type is QEvent::Gesture";
//        return gestureEvent(static_cast<QGestureEvent*>(event));
//    }
//    return QWidget::event(event);
//}

//bool MobileVis1MainWindow::gestureEvent(QGestureEvent *event) {
//    if (QGesture *swipe = event->gesture(Qt::SwipeGesture)) {
//        //swipeTriggered(static_cast<QSwipeGesture *>(swipe));
//        qDebug() << "swipe";
//        event->setAccepted(swipe, true);
//        return true;
//    } else if (QGesture *pan = event->gesture(Qt::PanGesture)) {
////        panTriggered(static_cast<QPanGesture *>(pan));
//        qDebug() << "pan";
//        event->setAccepted(pan, true);
//        return true;
//    }
//    if (QGesture *pinch = event->gesture(Qt::PinchGesture)) {
////        pinchTriggered(static_cast<QPinchGesture *>(pinch));
//        qDebug() << "pinch";
//        event->setAccepted(pinch, true);
//        return true;
//    }
//    return true;
//}

//void MobileVis1MainWindow::swipeTriggered(QSwipeGesture *gesture) {
//    qDebug() << "swipeTriggered";
//    if (gesture->state() == Qt::GestureFinished) {
//        if (gesture->horizontalDirection() == QSwipeGesture::Left)
//            qDebug() << "left";
//        if (gesture->horizontalDirection() == QSwipeGesture::Right)
//            qDebug() << "right";
//        if (gesture->verticalDirection() == QSwipeGesture::Up)
//            qDebug() << "up";
//        if (gesture->verticalDirection() == QSwipeGesture::Down)
//            qDebug() << "down";
//        update();
//    }
//}
