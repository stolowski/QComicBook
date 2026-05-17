/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005 Pawel Stolowski <pawel.stolowski@wp.pl>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "ImgCache.h"

using namespace QComicBook;

ImgCache::ImgCache(int size): maxItemSizeSoFar(0)
{
	setSize(size);
}

ImgCache::~ImgCache()
{
}

void ImgCache::setSize(int size, bool autoAdjust)
{
	mtx.lock();
	if (size < 0)
		size = 0;
	cache.setMaxCost(size);
	this->autoAdjust = autoAdjust;
	mtx.unlock();
}

void ImgCache::insertImage(int page, const QImage &img)
{
	mtx.lock();
	if (autoAdjust && (img.sizeInBytes() + maxItemSizeSoFar > cache.maxCost()))
	{
		cache.setMaxCost(img.sizeInBytes() + maxItemSizeSoFar);
		if (img.sizeInBytes() > maxItemSizeSoFar)
			maxItemSizeSoFar = img.sizeInBytes();
	}
	cache.insert(page, new QImage(img), img.sizeInBytes());
	mtx.unlock();
}

bool ImgCache::get(int num, QImage &img)
{
	mtx.lock();
	bool status = false;
	QImage *cimg = cache.object(num);
	if (cimg)
	{
		img = QImage(*cimg);
		status = true;
	}
	mtx.unlock();
	return status;
}

