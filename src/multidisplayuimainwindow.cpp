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

#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include <QSettings>
#include <QIcon>

#include "multidisplayuimainwindow.h"
#include "MdData.h"
#include "VisualizationPlot.h"
#include "evaluation/evaluationwindow.h"
#include "TransferFunction.h"
#include "widgets/Overlay.h"

MultidisplayUIMainWindow::MultidisplayUIMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow);
#endif

    statusBarRecordSizeLabel = new QLabel ("0");
    ui.StatusBar->addPermanentWidget(statusBarRecordSizeLabel);

    dfMapActionGroup = new QActionGroup(this);
    dfMapActionGroup->addAction(ui.action100kpa);
    dfMapActionGroup->addAction(ui.action200kpa);
    dfMapActionGroup->addAction(ui.action250kpa);
    dfMapActionGroup->addAction(ui.action300kpa);
    dfMapActionGroup->addAction(ui.action400kpa);

#ifdef Q_WS_X11
    //http://standards.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
    //http://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
    ui.PlayButton->setIcon(  QIcon::fromTheme ("media-playback-start") );
    ui.PlayButton->setText("");
    ui.PauseButton->setIcon(  QIcon::fromTheme ("media-playback-pause") );
    ui.PauseButton->setText("");
    ui.StopButton->setIcon(  QIcon::fromTheme ("media-playback-stop") );
    ui.StopButton->setText("");
#endif

    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::SwipeGesture);

    connect (ui.action100kpa, SIGNAL(triggered(bool)), this, SLOT(dfMapSelected100(bool)));
    connect (ui.action200kpa, SIGNAL(triggered(bool)), this, SLOT(dfMapSelected200(bool)));
    connect (ui.action250kpa, SIGNAL(triggered(bool)), this, SLOT(dfMapSelected250(bool)));
    connect (ui.action300kpa, SIGNAL(triggered(bool)), this, SLOT(dfMapSelected300(bool)));
    connect (ui.action400kpa, SIGNAL(triggered(bool)), this, SLOT(dfMapSelected400(bool)));

    overlay = NULL;
//    overlay = new Overlay (this);

    //ui.DataTab->setStyleSheet ("background: url(/home/bofh/1280x720dfo_yt_landscape.png)");
}


MultidisplayUIMainWindow::~MultidisplayUIMainWindow() {

}

void MultidisplayUIMainWindow::showStatusMessage (QString msg) {
    ui.StatusBar->showMessage(msg);
    ui.LogTextEdit->appendPlainText(msg);
}

void MultidisplayUIMainWindow::showStatusBarSampleCount (QString msg) {
    statusBarRecordSizeLabel->setText(msg);
}

void MultidisplayUIMainWindow::closeEvent(QCloseEvent *event) {
    emit writeSettings();
    event->accept();
    QMainWindow::closeEvent ( event );
}

void MultidisplayUIMainWindow::enableReplay() {
    ui.ReplayGroupBox->setChecked(true);
}

void MultidisplayUIMainWindow::disableReplay() {
    ui.ReplayGroupBox->setChecked(false);
}

void MultidisplayUIMainWindow::newDfBoostTransferFunction (int name) {
    switch (name) {
    case 200 :
        ui.action200kpa->setChecked(true);
        break;
    case 250 :
        ui.action250kpa->setChecked(true);
        break;
    case 100 :
        ui.action200kpa->setChecked(true);
        break;
    case 300 :
        ui.action300kpa->setChecked(true);
        break;
    case 400 :
        ui.action400kpa->setChecked(true);
        break;
    }
    QSettings settings;
    settings.setValue ("Digifant1/MapSensor", name );
}

void MultidisplayUIMainWindow::dfMapSelected100(bool b) {
    AppEngine::getInstance()->setDfBoostTransferFunction( new TransferFunction100kpa() );
}
void MultidisplayUIMainWindow::dfMapSelected200(bool b) {
    AppEngine::getInstance()->setDfBoostTransferFunction( new TransferFunction200kpa() );
}
void MultidisplayUIMainWindow::dfMapSelected250(bool b) {
    AppEngine::getInstance()->setDfBoostTransferFunction( new TransferFunction250kpa() );
}
void MultidisplayUIMainWindow::dfMapSelected300(bool b) {
    AppEngine::getInstance()->setDfBoostTransferFunction( new TransferFunction300kpa() );
}
void MultidisplayUIMainWindow::dfMapSelected400(bool b) {
    AppEngine::getInstance()->setDfBoostTransferFunction( new TransferFunction400kpa() );
}


void MultidisplayUIMainWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent( event );
    qDebug() << "MultidisplayUIMainWindow::resizeEvent old=" << event->oldSize() << "  new=" << event->size() ;

    /* overlay is not managed by the layout
      -> we must propagate resize information to it */
    if ( overlay )
        overlay->resize( event->size() );
}

void MultidisplayUIMainWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    connect (AppEngine::getInstance(), SIGNAL(newDfBoostTransferFunction(int)), this, SLOT(newDfBoostTransferFunction(int)));
}
