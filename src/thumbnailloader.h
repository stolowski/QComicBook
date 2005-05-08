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

/*! \file imgloader.h */

#ifndef __THUMBNAILLOADER_H
#define __THUMBNAILLOADER_H

#include "imgloader.h"

class QObject;

#define ThumbnailReady 2000

class ThumbnailLoaderThread: public ImgLoaderThread
{
	private:
		QObject *rcvobj; //!<object that will recieve loaded thumbnails
		volatile bool usecache;
		
	protected:
		virtual void run();

	public:
		ThumbnailLoaderThread();
		virtual ~ThumbnailLoaderThread();

		virtual void setReciever(QObject *rcv);
		virtual void setUseCache(bool f);
};

#endif

