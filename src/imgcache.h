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

#ifndef __IMGCACHE_H
#define __IMGCACHE_H

#include <qcache.h>
#include <qimage.h>

class ImgCache: public QCache<QImage>
{
	public:
		ImgCache(int size=1);
		virtual ~ImgCache();

		virtual void setSize(int size);
		//virtual int memory() const;
		//virtual void largestImage(int &w, int &h);
		virtual QPtrCollection::Item newItem(QPtrCollection::Item d);
};

#endif

