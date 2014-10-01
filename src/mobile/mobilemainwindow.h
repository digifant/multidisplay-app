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

#ifndef MOBILEMAINWINDOW_H
#define MOBILEMAINWINDOW_H

#include <QMainWindow>
#include "MdData.h"
#include "serialoptions.h"
#include "evaluation/evaluationwindow.h"

#if defined (Q_WS_MAEMO_5)
#include <QSystemScreenSaver>
QTM_USE_NAMESPACE
#endif

namespace Ui {
    class MobileMainWindow;
}

class MobileMainWindow : public QMainWindow
{
    Q_OBJECT

     friend class AppEngine;

public:
    explicit MobileMainWindow(QWidget *parent = 0);
    ~MobileMainWindow();

    void showExpanded();
    bool event(QEvent *event);
    bool gestureEvent(QGestureEvent *event);

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void writeSettings();

public slots:
    void showStatusMessage (QString msg);

private:
    Ui::MobileMainWindow *ui;
#if  defined (Q_WS_MAEMO_5)
    QSystemScreenSaver *screensaver;
#endif
};

#endif // MOBILEMAINWINDOW_H
