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

#include "imgcache.h"
#include <qptrcollection.h>

ImgCache::ImgCache(int size): QCache<QImage>(size)/*{{{*/
{
	if (size < 1)
		setSize(1);
	setAutoDelete(true);
}/*}}}*/

ImgCache::~ImgCache()/*{{{*/
{
}/*}}}*/

void ImgCache::setSize(int size)/*{{{*/
{
	setMaxCost(size);
}/*}}}*/

/*int ImgCache::memory() const
{
	int mem = 0;
	for (QCacheIterator<QImage> it(*this); it.current(); ++it)
		mem += it.current()->numBytes();
	return mem;	
};

void ImgCache::largestImage(int &w, int &h)
{
	int p;
	p = w = h = 0;
	for (QCacheIterator<QImage> it(*this); it.current(); ++it)
	{
		QImage *img = it.current();
		int tmp = img->width() * img->height();
		if (tmp > p)
		{
			p = tmp;
			w = img->width();
			h = img->height();
		}
	}
}*/

QPtrCollection::Item ImgCache::newItem(QPtrCollection::Item d)
{
	//std::cout << "img copy" << std::endl;
	return new QImage(*(QImage *)d);
}

