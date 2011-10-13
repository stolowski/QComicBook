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

#ifndef __FRAME_WIDGET_H
#define __FRAME_WIDGET_H

#include "ComicImageWidget.h"
#include <QRect>

class QImage;

namespace QComicBook
{
	class Page;
	class ComicFrame;
	class FrameView;

	class FrameWidget: public ComicImageWidget
	{
		public:
			FrameWidget(FrameView *parent, int w, int h);
			virtual ~FrameWidget();

			void setFrame(const Page &p, const ComicFrame &f);
			void clear();
			void redraw(QPainter &p);
                        ImageTransformJob *redrawJob();
                        bool jobCompleted(const ImageJobResult &result);
                        void propsChanged();

		private:
			QImage *m_image;
			QRect m_frame;
                        int m_framekey;
	};
}

#endif
