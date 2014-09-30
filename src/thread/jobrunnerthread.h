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

#ifndef JOBRUNNERTHREAD_H
#define JOBRUNNERTHREAD_H

#include <QObject>

class WorkerJob;
class QThread;


//! runs a Workerjob in a new Thread
class JobRunnerThread : public QObject
{
    Q_OBJECT
public:
    explicit JobRunnerThread(QObject *parent = 0, WorkerJob* job = 0 );

    WorkerJob* getWorkerJob() { return job; };

    bool isFinished () const;
    bool isRunning () const;

signals:
    void startWork();

    void finished ();
    void started ();
    void terminated ();

public slots:
    void start();

    void threadFinished ();
    void threadStarted ();
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    void threadTerminated ();
#endif


private:
    WorkerJob *job;
    QThread *t;
};

#endif // JOBRUNNERTHREAD_H
