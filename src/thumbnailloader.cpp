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

#include "thumbnailloader.h"
#include "imgsink.h"
#include "thumbnail.h"
#include <qapplication.h>
#include <qimage.h>
#include <qevent.h>

ThumbnailLoader::ThumbnailLoader(): QThread(), stopped(false), sink(NULL), rcvobj(NULL), usecache(true)/*{{{*/
{
}/*}}}*/

ThumbnailLoader::~ThumbnailLoader()/*{{{*/
{
}/*}}}*/

void ThumbnailLoader::setSink(ImgSink *sink)/*{{{*/
{
	mtx.lock();
	this->sink = sink;
	mtx.unlock();
}/*}}}*/

void ThumbnailLoader::setReciever(QObject *rcv)/*{{{*/
{
	mtx.lock();
	rcvobj = rcv;
	mtx.unlock();
}/*}}}*/

void ThumbnailLoader::setUseCache(bool f)/*{{{*/
{
	mtx.lock();
	usecache = f;
	mtx.unlock();
}/*}}}*/

void ThumbnailLoader::requestThumbnail(int num)/*{{{*/
{
	mtx.lock();
	if (requests.contains(num))
	{
		mtx.unlock();
		return;
	}
	requests.append(num);
	if (!running())
		start();
	mtx.unlock();
}/*}}}*/

void ThumbnailLoader::requestThumbnails(int first, int n)/*{{{*/
{
	mtx.lock();
	for (int i=first; i<n; i++)
		if (requests.contains(i) == 0)
			requests.append(i);
	if (!running())
		start();
	mtx.unlock();
}/*}}}*/

void ThumbnailLoader::stop()/*{{{*/
{
	mtx.lock();
	stopped = true;
	mtx.unlock();
}/*}}}*/

void ThumbnailLoader::run()/*{{{*/
{
	for (;;)
	{
		mtx.lock();
		if (stopped || requests.empty())
		{
			mtx.unlock();
			break;
		}
		int n = requests.first();
		requests.pop_front();
		if (sink && rcvobj)
		{
			Thumbnail *t = sink->getThumbnail(n, usecache);
			mtx.unlock();
			if (t)
			{
				QCustomEvent *evt = new QCustomEvent(ThumbnailReady);
				evt->setData(t);
				QApplication::postEvent(rcvobj, evt);
			}
		}
		else
			mtx.unlock();
	}
}/*}}}*/

