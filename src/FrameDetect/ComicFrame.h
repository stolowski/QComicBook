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

#ifndef __COMIC_FRAME_H
#define __COMIC_FRAME_H

namespace QComicBook
{
	class ComicFrame
	{
		public:
			ComicFrame(int x, int y, int w, int h, int label): x(x), y(y), w(w), h(h), lbl(label) {}
			~ComicFrame() {};

			int width() const { return w; }
			int height() const { return h; }
			int xPos() const { return x; }
			int yPos() const { return y; }
			int label() const { return lbl; }

		private:
			int x, y, w, h;
			int lbl;
	};
}

#endif
