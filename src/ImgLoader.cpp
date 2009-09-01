/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <pawel.stolowski@wp.pl>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "ImgLoader.h"
#include "ImgDirSink.h"
#include <iostream>
#include <QPixmap>

using namespace QComicBook;

ImgLoaderThread::ImgLoaderThread(): QThread(), prio(QThread::LowPriority), sink(NULL), stopped(false)
{
}

ImgLoaderThread::~ImgLoaderThread()
{
}

void ImgLoaderThread::setPriority(QThread::Priority p)
{
    loaderMutex.lock();
    prio = p;
    loaderMutex.unlock();
}

void ImgLoaderThread::setSink(ImgDirSink *sink)
{
    sinkMutex.lock();
    this->sink = sink;
    sinkMutex.unlock();
}

void ImgLoaderThread::request(int page)
{
    loaderMutex.lock();
    const LoadRequest req(page, false);
    if (requests.contains(req))
    {
        loaderMutex.unlock();
        return;
    }
    requests.append(req);
    loaderMutex.unlock();
    reqCond.wakeOne();
}

void ImgLoaderThread::requestTwoPages(int page)
{
    std::cout << "request 2 pages: " << page << "\n";

    loaderMutex.lock();
    const LoadRequest req(page, true);
    if (requests.contains(req))
    {
        loaderMutex.unlock();
        return;
    }
    requests.append(req);
    loaderMutex.unlock();
    reqCond.wakeOne();
}

void ImgLoaderThread::request(int first, int n)
{
    for (int i=first; i<first+n; i++)
    {
        request(i);
    }
}

void ImgLoaderThread::cancel(int page)
{
    loaderMutex.lock();
    const LoadRequest req(page, false);
    requests.removeAll(req);
    loaderMutex.unlock();
}

void ImgLoaderThread::cancelTwoPages(int page)
{
    loaderMutex.lock();
    const LoadRequest req(page, true);
    requests.removeAll(req);
    loaderMutex.unlock();
}

void ImgLoaderThread::cancelAll()
{
    loaderMutex.lock();
    requests.clear();
    loaderMutex.unlock();
}

void ImgLoaderThread::stop()
{
    loaderMutex.lock();
    stopped = true;
    loaderMutex.unlock();
    reqCond.wakeOne();
}

void ImgLoaderThread::run()
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

            sinkMutex.lock();
            if (sink) //TODO not safe, sink as shared ptr
            {
                int result;
                std::cout << "preloading: " << req.pageNumber << std::endl;
                if (req.twoPages)
                {
                    const Page page1(sink->getImage(req.pageNumber, result));
                    const Page page2(sink->getImage(req.pageNumber+1, result));
                    sinkMutex.unlock();
                    emit pageLoaded(page1, page2); //TODO errors
                }
                else
                {
                    const Page page(sink->getImage(req.pageNumber, result));
                    sinkMutex.unlock();
                    emit pageLoaded(page);
                }
            }
            else
            {
                sinkMutex.unlock();
            }
	}
    }
}

