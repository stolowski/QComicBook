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

#include "imgloaderthread.h"

class QObject;

#define ThumbnailReady 2000

class ThumbnailLoader: public ImgLoaderThread
{
	private:
		QObject *rcvobj;
		volatile bool usecache;
		
	protected:
		virtual void run();

	public:
		ThumbnailLoader();
		virtual ~ThumbnailLoader();

		void setReciever(QObject *rcv);
		void setUseCache(bool f);
};

#endif

