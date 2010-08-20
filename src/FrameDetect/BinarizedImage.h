/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef BINARIZEDIMAGE_H
#define BINARIZEDIMAGE_H

#include <AbstractImage.h>

class QImage;

namespace QComicBook
{
	class BinarizedImage: public AbstractImage<unsigned char>
	{
		public:
			BinarizedImage(const QImage &img, int thmin);
			void invert();

			QImage toImage();
	};
}

#endif // BINARIZEDIMAGE_H
