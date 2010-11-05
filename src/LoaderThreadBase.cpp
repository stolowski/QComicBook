/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "LoaderThreadBase.h"
#include "ImgSink.h"
#include <QPixmap>
#include <QtDebug>

using namespace QComicBook;
LoaderThreadBase::LoaderThreadBase(): QThread(), prio(QThread::LowPriority), sink(NULL), stopped(false)
{
}

LoaderThreadBase::~LoaderThreadBase()
{
}

void LoaderThreadBase::setPriority(QThread::Priority p)
{
    loaderMutex.lock();
    prio = p;
    loaderMutex.unlock();
}

void LoaderThreadBase::setSink(ImgSink *sink)
{
    sinkMutex.lock();
    this->sink = sink;
    sinkMutex.unlock();
}

void LoaderThreadBase::request(int page)
{
    qDebug() << "requested page" << page;

    loaderMutex.lock();
    const LoadRequest req(page, false);
    if (requests.contains(req))
    {
        qDebug() << "requests queue already has" << page;
        loaderMutex.unlock();
        return;
    }
    requests.append(req);
    loaderMutex.unlock();
    reqCond.wakeOne();
}

void LoaderThreadBase::requestTwoPages(int page)
{
    qDebug() << "requested 2 pages" << page;

    loaderMutex.lock();
    const LoadRequest req(page, true);
    if (requests.contains(req))
    {
        qDebug() << "requests queue (2 pages) already has" << page;
        loaderMutex.unlock();
        return;
    }
    requests.append(req);
    loaderMutex.unlock();
    reqCond.wakeOne();
}

void LoaderThreadBase::request(int first, int n)
{
    for (int i=first; i<first+n; i++)
    {
        request(i);
    }
}

void LoaderThreadBase::cancel(int page)
{
    loaderMutex.lock();
    const LoadRequest req(page, false);
    qDebug() << "canceled page" << page;
    requests.removeAll(req);
    loaderMutex.unlock();
}

void LoaderThreadBase::cancelTwoPages(int page)
{
    loaderMutex.lock();
    qDebug() << "canceled 2 pages" << page;
    const LoadRequest req(page, true);
    requests.removeAll(req);
    loaderMutex.unlock();
}

void LoaderThreadBase::cancelAll()
{
    loaderMutex.lock();
    qDebug() << "canceled all pages";
    requests.clear();
    loaderMutex.unlock();
}

void LoaderThreadBase::stop()
{
    loaderMutex.lock();
    stopped = true;
    loaderMutex.unlock();
    reqCond.wakeOne();
}

void LoaderThreadBase::run()
{
    for (;;)
    {
        condMutex.lock();
        reqCond.wait(&condMutex);
        condMutex.unlock();
        	
        for (;;) {
            loaderMutex.lock();
            if (stopped)
            {
                loaderMutex.unlock();
                return;
            }

            if (requests.empty())
            {
                loaderMutex.unlock();
                break;
            }
            const LoadRequest req(requests.first());
            requests.pop_front();
            loaderMutex.unlock();

            sinkMutex.lock(); //TODO is it safe to lock when process() may emit signal?
            if (sink)
            {
                qDebug() << "loading" << req.pageNumber;
                process(req);
            }
            sinkMutex.unlock();
	}
    }
}

