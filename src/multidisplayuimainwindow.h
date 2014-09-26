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

#ifndef MULTIDISPLAYUIMAINWINDOW_H
#define MULTIDISPLAYUIMAINWINDOW_H

#if QT_VERSION >= 0x050000
#include <QMainWindow>
#else
#include <QtGui/QMainWindow>
#endif
#include <QActionGroup>
#include "ui_multidisplayuimainwindow.h"
#include "serialoptions.h"
#include "AppEngine.h"

class MdSerialCom;
class MdData;
class CarPcDemoWidget;
class EvaluationWindow;
class QLabel;
class MdBoostPidRecord;
class Overlay;

class MultidisplayUIMainWindow : public QMainWindow
{
    Q_OBJECT

    friend class AppEngine;

public:
    MultidisplayUIMainWindow(QWidget *parent = 0);
    ~MultidisplayUIMainWindow();

signals:
    void writeSettings();

public slots:
    void showStatusMessage (QString msg);
    void showStatusBarSampleCount (QString msg);
    void enableReplay();
    void disableReplay();
    void newDfBoostTransferFunction (int name);

    void dfMapSelected100(bool b);
    void dfMapSelected200(bool b);
    void dfMapSelected250(bool b);
    void dfMapSelected300(bool b);
    void dfMapSelected400(bool b);

protected:
    void closeEvent ( QCloseEvent * event );
    virtual void resizeEvent(QResizeEvent *event);

private:
    Ui::MultidisplayUIMainWindowClass ui;
    QLabel *statusBarRecordSizeLabel;
    QActionGroup *dfMapActionGroup;

    Overlay* overlay;
};

#endif // MULTIDISPLAYUIMAINWINDOW_H
