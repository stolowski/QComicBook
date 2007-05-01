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

/*! \file imgcache.h */

#ifndef __IMGCACHE_H
#define __IMGCACHE_H

#include <QCache>
#include <QImage>
#include <QMutex>

namespace QComicBook
{
	class ImgCache
	{
		private:
			QCache<int, QImage> cache;
			bool autoAdjust;
			int maxItemSizeSoFar;
			QMutex mtx;

		public:
			ImgCache(int size=1);
			virtual ~ImgCache();

			virtual void setSize(int size, bool autoAdjust=false);
			void insertImage(int page, const QImage &img);
			bool get(int num, QImage &img);
	};
}

#endif

