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

#ifndef __COMIC_FRAME_LIST_H
#define __COMIC_FRAME_LIST_H

#include <QVector>
#include <ComicFrame.h>

namespace QComicBook
{
	class ComicFrameList
	{
		public:
			ComicFrameList(int page=-1, int width=0, int height=0);
			virtual ~ComicFrameList();

			void append(const ComicFrame &f);
			void sort();
			const ComicFrame& operator[](int idx);
			int count() const { return m_frames.count(); }
			int pageNumber() const { return m_page; }

		private:
			int m_page;
			int m_pageWidth;
			int m_pageHeight;
			QVector<ComicFrame> m_frames;
	};
}

#endif
