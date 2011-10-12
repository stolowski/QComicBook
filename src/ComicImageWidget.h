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
#include <QGraphicsItem>
#include <QPixmap>

class QPixmap;
class QPainter;

namespace QComicBook
{
	class PageViewBase;
        class ImageTransformJob;
        class ImageJobResult;

	class ComicImageWidget: public QGraphicsItem
	{
		public:
			ComicImageWidget(PageViewBase *parent);
			virtual ~ComicImageWidget();
        
			virtual void dispose();
			virtual bool isDisposed() const;
        
        	bool isInView(int vy1, int vy2) const;
			void setSourceSize(int w, int h);
			QSize getSourceSize() const;
			QSize getScaledSize() const;
			const QPixmap* pixmap() const;
        	QRectF boundingRect() const;
	        void paint(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *widget = 0);
			void redrawScaledImage();
			PageViewBase* view() const;
			void recalcScaledSize();
			int width() const;
			int height() const;

                        virtual void jobCompleted(const ImageJobResult &result);
			virtual void propsChanged() = 0;

		protected:
			virtual void redraw(QPainter &p) = 0;
                        virtual ImageTransformJob* redrawJob() = 0;

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
