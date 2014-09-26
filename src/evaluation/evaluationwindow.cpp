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

#include <QDebug>
#include "evaluationwindow.h"
#include "EvalSpectrogramPlot.h"

//EvaluationWindow::EvaluationWindow(Ui::MultidisplayUIMainWindowClass *main_ui, MdData* data, Type type)
//    : QWidget(NULL), main_ui(main_ui), data(data), ep(NULL)
EvaluationWindow::EvaluationWindow(QWidget *parent, MdData* data, Type type)
    : QDialog(parent), data(data), ep(NULL)

{

    ui.setupUi(this);

//#ifdef Q_WS_MAEMO_5
//    setAttribute(Qt::WA_Maemo5StackedWindow);
//#endif

    switch (type) {
    case BoostLambda :
        ep = new EvalPlotBoostLambda ((QMainWindow*) this, (QWidget*) ui.PlotFrame);
        break;
    case RPMBoost :
        ep = new EvalPlotRPMBoost ((QMainWindow*) this, (QWidget*) ui.PlotFrame);
        break;
    case SpectroBoostLambda :
        ep = new EvalSpectrogramPlot ((QMainWindow*) this, (QWidget*) ui.PlotFrame);
        break;
    default:
        ep = NULL;
    }

    connect (ui.OkButton, SIGNAL(clicked()), this, SLOT(hide() ) );
}



EvaluationWindow::~EvaluationWindow()
{
    delete (ep);
}

void EvaluationWindow::showEvent ( QShowEvent * event ) {
    if ( !(event->spontaneous()) && (event->type() == QEvent::Show) ) {
        ep->compute(data);
        qDebug() << "void EvaluationWindow::showEvent ( QShowEvent * event ) compute called";
    }
}
