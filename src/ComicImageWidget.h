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

#ifndef __COMIC_IMAGE_WIDGET_H
#define __COMIC_IMAGE_WIDGET_H

#include <QWidget>
#include <QSize>
#include <QMatrix>

class QPixmap;
class QPainter;

namespace QComicBook
{
	class PageViewBase;

	class ComicImageWidget: public QWidget
	{
    Q_OBJECT

		public:
			ComicImageWidget(PageViewBase *parent, int w, int h);
			virtual ~ComicImageWidget();
        
			virtual void dispose();
			virtual bool isDisposed() const;
        
			void setSourceSize(int w, int h);
			QSize getSourceSize() const;
			QSize getScaledSize() const;
			const QPixmap* pixmap() const;
			virtual void paintEvent(QPaintEvent *event);
			void redrawScaledImage();
			PageViewBase* view() const;
			void recalcScaledSize();

		protected:
			virtual void redraw(QPainter &p) = 0;

		private:
			PageViewBase *m_view;
			QPixmap *m_pixmap;
			int xoff, yoff;
			QMatrix rmtx;
			QSize imageSize; //image size without scaling
			QSize scaledSize; //image size with scaling and rotation
	};
}

#endif
