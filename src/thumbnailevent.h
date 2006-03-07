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

#ifndef __THUMBNAILEVENT_H
#define __THUMBNAILEVENT_H

/*! \file thumbnailevent.h */

#include <qevent.h>

#define ThumbnailReady 65111

namespace QComicBook
{

	class Thumbnail;

	class ThumbnailEvent: public QCustomEvent
	{
		private:
			Thumbnail *thumbnail;

		public:
			ThumbnailEvent(Thumbnail *t);
			virtual ~ThumbnailEvent();

			const Thumbnail *getThumbnail() const;
	};
}

#endif

