/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <yogin@linux.bydg.org>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "imgloader.h"
#include "imgdirsink.h"
#include <qimage.h>

using namespace QComicBook;

ImgLoaderThread::ImgLoaderThread(): QThread(), prio(QThread::LowPriority), sink(NULL), stopped(false)
{
}

ImgLoaderThread::~ImgLoaderThread()
{
}

void ImgLoaderThread::setPriority(QThread::Priority p)
{
	mtx.lock();
	prio = p;
	mtx.unlock();
}

void ImgLoaderThread::setSink(ImgDirSink *sink)
{
	mtx.lock();
	this->sink = sink;
	mtx.unlock();
}

void ImgLoaderThread::request(int page)
{
	mtx.lock();
	if (requests.contains(page))
	{
		mtx.unlock();
		return;
	}
	requests.append(page);
	if (!isRunning() && !stopped)
	{
		mtx.unlock();
		start(prio);
	}
	else
		mtx.unlock();
}

void ImgLoaderThread::request(int first, int n)
{
	mtx.lock();
	const int last = first + n;
	for (int i=first; i<last; i++)
		if (requests.contains(i) == 0)
			requests.append(i);
	if (!isRunning() && !stopped)
	{
		mtx.unlock();
		start(prio);
	}
	else
		mtx.unlock();
}

void ImgLoaderThread::stop()
{
	mtx.lock();
	stopped = true;
	mtx.unlock();
}

void ImgLoaderThread::run()
{
	for (;;)
	{
		mtx.lock();
		if (stopped || requests.empty())
		{
			mtx.unlock();
			break;
		}
		
		const int n = requests.first();
		requests.pop_front();
		if (sink)
		{
			int result;
			sink->getImage(n, result);
		}
		mtx.unlock();
	}
}

