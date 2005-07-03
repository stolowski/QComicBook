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
#include "thumbnailevent.h"
#include <qapplication.h>

ThumbnailLoaderThread::ThumbnailLoaderThread(): ImgLoaderThread(), rcvobj(NULL), usecache(false)
{
}

ThumbnailLoaderThread::~ThumbnailLoaderThread()
{
}

void ThumbnailLoaderThread::setReciever(QObject *rcv)
{
	mtx.lock();
	rcvobj = rcv;
	mtx.unlock();
}

void ThumbnailLoaderThread::setUseCache(bool f)
{
	mtx.lock();
	usecache = f;
	mtx.unlock();
}

void ThumbnailLoaderThread::run()
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
				ThumbnailEvent *evt = new ThumbnailEvent(t);
				QApplication::postEvent(rcvobj, evt);
			}
		}
		else
			mtx.unlock();
	}
}

