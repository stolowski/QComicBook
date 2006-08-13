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

#ifndef __THUMBNAIL_H
#define __THUMBNAIL_H

#include <qimage.h>

namespace QComicBook
{
	class Thumbnail
	{
		private:
			int num;
			QImage img;
			static int thwidth, thheight; //default thumbnail width and height

		public:
			Thumbnail(int n);
			Thumbnail(int n, const QImage &i);
			~Thumbnail();

			int page() const;
			const QImage& image() const;
			void touch(const QString &fname);
			bool tryLoad(const QString &fname);
			bool save(const QString &fname);
			void setImage(const QImage &i);

			static int maxWidth();
			static int maxHeight();
	};
}

#endif

