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

#ifndef EVALUATIONWINDOW_H
#define EVALUATIONWINDOW_H

#if QT_VERSION >= 0x050000
#include <QWidget>
#else
#include <QtGui/QWidget>
#endif
#include "ui_evaluationwindow.h"
#include "evaluation/EvalPlot.h"
#include "MdData.h"
#include <QDialog>

class EvaluationWindow : public QDialog
{
    Q_OBJECT

public:
    enum Type
    {
    	BoostLambda,
    	RPMBoost,
    	SpectroBoostLambda
    };


//    EvaluationWindow ( Ui::MultidisplayUIMainWindowClass *main_ui, MdData* data = 0, Type type = BoostLambda);
    EvaluationWindow ( QWidget *parent, MdData* data = 0, Type type = BoostLambda);

    ~EvaluationWindow();

    void showEvent ( QShowEvent * event );

signals:
    void writeSettings ();

private:
    Ui::EvaluationMainWindow ui;
//    Ui::MultidisplayUIMainWindowClass *main_ui;
    MdData* data;
    EvalPlot* ep;
};

#endif // EVALUATIONWINDOW_H
