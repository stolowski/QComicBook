/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <pawel.stolowski@wp.pl>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "thumbnailitem.h"
#include "thumbnail.h"

using namespace QComicBook;

ThumbnailItem::ThumbnailItem(int page): pnum(page), loaded(false)
{
}

ThumbnailItem::~ThumbnailItem()
{
}

int ThumbnailItem::page() const
{
	return pnum;
}

void ThumbnailItem::setLoaded(bool f)
{
	loaded = f;
}

bool ThumbnailItem::isLoaded() const
{
	return loaded;
}

void ThumbnailItem::setThumbnail(const Thumbnail *t)
{
}

