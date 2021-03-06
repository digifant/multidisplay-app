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

#ifndef WORKERJOB_H
#define WORKERJOB_H

#include <QObject>


//! abstract base class of a worker job which gets executed in its own thread
class WorkerJob : public QObject
{
    Q_OBJECT
public:
    explicit WorkerJob(QObject *parent = 0);
    virtual ~WorkerJob();

signals:
    void jobFinished();

public slots:
    virtual void start() = 0;
    virtual void stop();
    virtual void pause();
};

#endif // WORKERJOB_H
