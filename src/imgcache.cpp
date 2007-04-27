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
#include <iostream>

using namespace QComicBook;

ImgCache::ImgCache(int size): QCache<int, QImage>(size)
{
}

ImgCache::~ImgCache()
{
}

void ImgCache::setSize(int size)
{
	std::cout << "cache size=" << size << std::endl;
        if (size < 0)
                size = 0;
        setMaxCost(size);
}

void ImgCache::insertImage(int page, QImage *img)
{
	insert(page, img, img->numBytes());
}

