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

#ifndef MOBILEVIS1MAINWINDOW_H
#define MOBILEVIS1MAINWINDOW_H

#include <QMainWindow>
#include <QGestureEvent>
#include <QSwipeGesture>
//#include "mobile/N900SwipeGesture.h"

namespace Ui {
    class MobileVis1MainWindow;
}

class MobileVis1MainWindow : public QMainWindow
{
    Q_OBJECT

     friend class AppEngine;

public:
    explicit MobileVis1MainWindow(QWidget *parent = 0);
    ~MobileVis1MainWindow();

//     bool event(QEvent *event);
//     bool gestureEvent(QGestureEvent *event);
//     void swipeTriggered(QSwipeGesture *gesture);

public slots:
    void enableReplay();
    void disableReplay();

protected:
//    void swipe(N900SwipeGesture::SwipeDirection direction);

private:
    Ui::MobileVis1MainWindow *ui;
//    N900SwipeGesture *swipeGesture;
};

#endif // MOBILEVIS1MAINWINDOW_H
