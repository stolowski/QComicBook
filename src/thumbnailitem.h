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

#ifndef __THUMBNAILITEM_H
#define __THUMBNAILITEM_H

namespace QComicBook
{
	class Thumbnail;

	class ThumbnailItem
	{
		protected:
			int pnum; //page number
			bool loaded; //has image or is it empty?

		public:
			ThumbnailItem(int page);
			virtual ~ThumbnailItem();

			virtual int page() const;
			virtual void setLoaded(bool f);
			virtual bool isLoaded() const;

			virtual void setThumbnail(const Thumbnail *t);
	};
}

#endif

