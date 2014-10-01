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

#include "mobilemainwindow.h"
#include "ui_mobilemainwindow.h"

#include <QDebug>
#include <QGestureEvent>

#ifdef Q_WS_MAEMO_5
    #include <QtMaemo5>
    #include <QMaemo5InformationBox>
#endif


MobileMainWindow::MobileMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MobileMainWindow)
{
    ui->setupUi(this);
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow);

    //broken on N900 !
    //http://doc.qt.nokia.com/qt-maemo-4.6/gestures-overview.html
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::SwipeGesture);
#endif

#if  defined (Q_WS_MAEMO_5)
    screensaver = new QSystemScreenSaver (this);
    screensaver->setScreenSaverInhibit ();
    qDebug() << "Screensaver deactivated";
#endif

}

MobileMainWindow::~MobileMainWindow()
{
    delete ui;
}


void MobileMainWindow::showStatusMessage (QString msg) {
    //http://doc.qt.nokia.com/qt-maemo-4.6/qmaemo5informationbox.html#details
    qDebug() << "status msg " << msg;
#ifdef Q_WS_MAEMO_5
    QMaemo5InformationBox::information ( this, msg, 0 );
#endif
}

void MobileMainWindow::closeEvent(QCloseEvent *event) {
    emit writeSettings();
    event->accept();
    QMainWindow::closeEvent ( event );
}

bool MobileMainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture) {
        qDebug() << "gesture";
        return gestureEvent(static_cast<QGestureEvent*>(event));
    }
    return QWidget::event(event);
}

bool MobileMainWindow::gestureEvent(QGestureEvent *event)
{
    if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
//        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
        qDebug() << "swipe";
    else if (QGesture *pan = event->gesture(Qt::PanGesture))
//        panTriggered(static_cast<QPanGesture *>(pan));
        qDebug() << "pan";
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
//        pinchTriggered(static_cast<QPinchGesture *>(pinch));
        qDebug() << "pinch";
    return true;
}

void MobileMainWindow::showExpanded() {
#ifdef Q_OS_SYMBIAN
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}
