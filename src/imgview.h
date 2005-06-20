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

#ifndef __IMGVIEW_H
#define __IMGVIEW_H

#include <qscrollview.h>
#include <qwmatrix.h>

class QPopupMenu;
class QImage;
class QPixmap;
class QColor;
class QCursor;

class ComicImageView: public QScrollView
{
	Q_OBJECT

	public:
		enum Size { Original, FitWidth, FitHeight, WholePage, BestFit };
		enum Scaling { Smooth, Fast };
		enum Rotation { None, Left, Right };

	private:
		QPopupMenu *context_menu;
		QImage *orgimage; //original 1:1 image
		QPixmap *pixmap;
		Size isize;
		Scaling iscaling;
		int iangle; //rotation angle, 0..3, multipled by 90
		QWMatrix rmtx; //rotation matrix
		int spdx, spdy; //scroll speed
		int xoff, yoff;
		int lx, ly; //last mouse position when tracking mouse moves
		int wheelupcnt, wheeldowncnt;
		QCursor *smallcursor;
		static const int EXTRA_WHEEL_SPIN; //number of extra wheel spins to flip the page
		
	signals:
		void bottomReached();
		void topReached();

	protected:
		void resizeEvent(QResizeEvent *e);
		void updateImageSize();
		virtual void contentsContextMenuEvent(QContextMenuEvent *e);
		virtual void contentsWheelEvent(QWheelEvent *e);
		virtual void contentsMouseMoveEvent(QMouseEvent *e);
		virtual void contentsMousePressEvent(QMouseEvent *e);
		virtual void contentsMouseReleaseEvent(QMouseEvent *e);
		virtual void drawContents(QPainter *p, int clipx, int clipy, int clipw, int cliph);
		
	public slots:
		void setImage(const QImage &img, bool preserveangle=false);
		void setImage(const QImage &img1, const QImage &img2, bool preserveangle=false);
		void setScaling(ComicImageView::Scaling s);
		void setRotation(ComicImageView::Rotation r);
		void setSize(Size s);
		void setSizeOriginal();
		void setSizeFitWidth();
		void setSizeFitHeight();
		void setSizeWholePage();
		void setSizeBestFit();
		void scrollToTop();
		void scrollToBottom();
		void scrollRight();
		void scrollLeft();
		void scrollRightFast();
		void scrollLeftFast();
		void scrollUp();
		void scrollDown();
		void scrollUpFast();
		void scrollDownFast();
		void rotateRight();
		void rotateLeft();
		void resetRotation();
		void jumpUp();
		void jumpDown();
		void clear();
		void enableScrollbars(bool f);
		void setBackground(const QColor &color);
		void setSmallCursor(bool f);

	public:
		ComicImageView(QWidget *parent, ComicImageView::Size size=Original, ComicImageView::Scaling scaling=Smooth, const QColor &color=Qt::black);
		~ComicImageView();
		bool onBottom();
		bool onTop();
		QPopupMenu *contextMenu() const;
		ComicImageView::Size getSize() const;
		const QPixmap& image() const;
};

#endif

