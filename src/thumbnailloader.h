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

#ifndef __THUMBNAILLOADER_H
#define __THUMBNAILLOADER_H

#include <qthread.h>
#include <qmutex.h>
#include <qvaluelist.h>

class ImgSink;
class QImage;
class QObject;

#define ThumbnailReady 2000

class ThumbnailLoader: public QThread
{
	private:
		ImgSink *sink;
		QObject *rcvobj;
		QMutex mtx;
		volatile bool stopped;
		volatile bool usecache;
		QValueList<int> requests;
		
	protected:
		virtual void run();

	public:
		ThumbnailLoader();
		~ThumbnailLoader();

		void setSink(ImgSink *sink);
		void setReciever(QObject *rcv);
		void setUseCache(bool f);
		void requestThumbnail(int num);
		void requestThumbnails(int first, int n);
		void stop();
};

#endif

