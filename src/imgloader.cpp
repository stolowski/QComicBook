/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005 Pawel Stolowski <yogin@linux.bydg.org>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "imgloader.h"
#include <imgsink.h>
#include <qimage.h>

ImgLoaderThread::ImgLoaderThread(): QThread(), prio(QThread::LowPriority), sink(NULL)/*{{{*/
{
}/*}}}*/

ImgLoaderThread::~ImgLoaderThread()/*{{{*/
{
}/*}}}*/

void ImgLoaderThread::setPriority(QThread::Priority p)/*{{{*/
{
	mtx.lock();
	prio = p;
	mtx.unlock();
}/*}}}*/

void ImgLoaderThread::setSink(ImgSink *sink)/*{{{*/
{
	mtx.lock();
	this->sink = sink;
	mtx.unlock();
}/*}}}*/

void ImgLoaderThread::request(int page)/*{{{*/
{
	mtx.lock();
	if (requests.contains(page))
	{
		mtx.unlock();
		return;
	}
	requests.append(page);
	if (!running())
		start(prio);
	mtx.unlock();
}/*}}}*/

void ImgLoaderThread::request(int first, int n)/*{{{*/
{
	mtx.lock();
	for (int i=first; i<n; i++)
		if (requests.contains(i) == 0)
			requests.append(i);
	if (!running())
		start(prio);
	mtx.unlock();
}/*}}}*/

void ImgLoaderThread::stop()/*{{{*/
{
	mtx.lock();
	stopped = true;
	mtx.unlock();

}/*}}}*/

void ImgLoaderThread::run()/*{{{*/
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
			sink->getImage(n, result, 0);
			mtx.unlock();
		}
		else
		{
			mtx.unlock();
		}
	}
}/*}}}*/

