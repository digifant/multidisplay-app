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

#include "replayworker.h"

#include <qtimer.h>
#include <qdebug.h>
#include <qthread.h>
#include "AppEngine.h"
#include "MdData.h"
#include "VisualizationPlot.h"

ReplayWorker::ReplayWorker(AppEngine* a) : controller(a), replayDone(false), t(new QTimer(this)), replayIsPaused(false), running(false), replayedItemCount(0) {
    connect (t, SIGNAL(timeout()), this, SLOT(update()));
    qDebug() << "ReplayWorker created in Thread " << QThread::currentThreadId() ;
}
ReplayWorker::~ReplayWorker () {
    if (t) {
        t->stop();
        delete (t);
        t = 0;
    }
}

void ReplayWorker::start() {
    qDebug() << "ReplayWorker started work in Thread " << QThread::currentThreadId() ;

    if ( !controller )
        return;

    if ( replayIsPaused ) {
        resume();
        return;
    }

    int startAtPos = controller->getReplayStartAtPos();

    QList<MdDataRecord*> *dataList = &(controller->getData()->getData());

    if (dataList->isEmpty())
        return;

    //clear Plots
    emit clearPlots();

    replayIt = dataList->begin();

    replayIt = dataList->begin();
    replayIsPaused = false;
    running = true;


    t->setSingleShot(true);


    if ( startAtPos ) {
        if ( startAtPos < dataList->size() ) {
            for ( int i = 0 ; i < startAtPos ; i++ ) {
                emit visualizeDataRecord( *replayIt, false );
                replayIt++;
                replayedItemCount++;
            }
        }
    }

    update();
}

void ReplayWorker::update() {
    if (replayIsPaused)
        return;

    MdDataRecord* dr = *replayIt;
    emit visualizeDataRecord( dr, true );

    QList<MdDataRecord*> *dataList = &(controller->getData()->getData());

    replayIt++;
    replayedItemCount++;
    if ( replayIt != dataList->end() ) {
        if ( (*replayIt)->getSensorR() != NULL ) {
            int next = ( (*replayIt)->getSensorR()->getTime() - dr->getSensorR()->getTime() ) / controller->getReplaySpeedUpFactor();

#ifndef Q_WS_MAEMO_5
            emit showStatusMessage( QString ("replay mode (") + QString::number(replayedItemCount) + QString (" / ") + QString::number(dataList->count()) + QString(")") );
#endif
            t->start (next);
        }
    } else {
        emit showStatusMessage("replay finished");
        emit jobFinished();
        replayedItemCount = 0;
        running = false;
    }
}

void ReplayWorker::pause () {
    if ( !replayIsPaused )
        replayIsPaused = true;
    else
        resume();
}

void ReplayWorker::resume () {
    if ( replayIsPaused && running ) {
        replayIsPaused = false;
        update();
    }
}

void ReplayWorker::stop () {
    t->stop();
    replayIsPaused = false;
    replayedItemCount = 0;

    QList<MdDataRecord*> *dataList = &(controller->getData()->getData());

    for ( QList<MdDataRecord*>::iterator it = replayIt ; it != dataList->end() ; it++ ) {
        if ( *it == dataList->last() )
            visualizeDataRecord( *it, true );
        else
            visualizeDataRecord( *it, false );
    }
    replayIt = dataList->end();
    t->stop();
    running = false;
}
