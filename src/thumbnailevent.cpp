/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <yogin@linux.bydg.org>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "thumbnailevent.h"
#include "thumbnail.h"

using namespace QComicBook;

ThumbnailEvent::ThumbnailEvent(Thumbnail *t): QEvent((QEvent::Type)ThumbnailReady), thumbnail(t)
{
}

ThumbnailEvent::~ThumbnailEvent()
{
	delete thumbnail;
}

const Thumbnail *ThumbnailEvent::getThumbnail() const
{
	return thumbnail;
}

