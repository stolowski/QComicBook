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

#ifndef __IMLIBIMAGE_H
#define __IMLIBIMAGE_H

/*! \file imlibimage.h */

#include <qmutex.h>

class QPaintDevice;
class QString;

namespace QComicBook
{
	class ImlibImage
	{
		private:
			void *data;
			void *context;
			int w, h;
			static QMutex mutex;

		public:
			ImlibImage();
			~ImlibImage();
			int load(const QString &path);
			void draw(QPaintDevice *p, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh);
			int width() const;
			int height() const;
			void rotate(int orient);
			ImlibImage* rotateClone(int orient);
			void reset();

			static void setCacheSize(int bytes);
	};
};

#endif

