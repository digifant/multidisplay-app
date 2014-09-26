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

#include <QThread>

#include "jobrunnerthread.h"
#include "workerjob.h"

JobRunnerThread::JobRunnerThread(QObject *parent, WorkerJob *job) :
    QObject(parent), job(job)
{

    t = new QThread();
    connect (t, SIGNAL(started()), this, SLOT(threadStarted()));
    connect (t, SIGNAL(finished()), this, SLOT(threadFinished()));
    connect (t, SIGNAL(terminated()), this, SLOT(threadTerminated()));

    if ( job ) {
        connect (this, SIGNAL(startWork()), job, SLOT(start()) );
        job->moveToThread(t);
        t->start();
    }
}

void JobRunnerThread::start() {
    emit startWork();
}

bool JobRunnerThread::isFinished () const {
    return t->isFinished();
}
bool JobRunnerThread::isRunning () const {
    return t->isRunning();
}

void JobRunnerThread::threadFinished () {
    emit finished();
}

void JobRunnerThread::threadStarted () {
    emit started();
}

void JobRunnerThread::threadTerminated () {
    emit terminated();
}
