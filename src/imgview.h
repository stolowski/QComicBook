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

#include <QScrollArea>
#include <QMatrix>
#include <QImage>

class QMenu;
class QPixmap;
class QColor;
class QCursor;
class QLabel;

namespace QComicBook
{
	enum Size { Original, FitWidth, FitHeight, WholePage, BestFit };
	enum Scaling { Smooth, Fast };
	enum Rotation { None, Left, Right };

	class ComicImageView: public QScrollArea
	{
		Q_OBJECT

		private:
			QMenu *context_menu;
			Size isize;
			QImage orgimg[2];
			int imgs; //number of images in orgimg array
			int iangle; //rotation angle, 0..3, multipled by 90
			QMatrix rmtx; //rotation matrix
			int spdx, spdy; //scroll speed
			int xoff, yoff;
			int lx, ly; //last mouse position when tracking mouse moves
			int wheelupcnt, wheeldowncnt;
			QCursor *smallcursor;
			static const int EXTRA_WHEEL_SPIN; //number of extra wheel spins to flip the page
			QLabel *imgLabel;
			int totalWidth;
			int totalHeight;

		signals:
			void bottomReached();
			void topReached();

		protected:
			void resizeEvent(QResizeEvent *e);
			void updateImageSize();
			void redrawImages();
			virtual void contextMenuEvent(QContextMenuEvent *e);
			virtual void wheelEvent(QWheelEvent *e);
			virtual void mouseMoveEvent(QMouseEvent *e);
			virtual void mousePressEvent(QMouseEvent *e);
			virtual void mouseReleaseEvent(QMouseEvent *e);
			void scrollByDelta(int dx, int dy);

		public slots:
			
			void setImage(const QImage &img1, bool preserveangle=false);
			void setImage(const QImage &img1, const QImage &img2, bool preserveangle=false);
			void setRotation(Rotation r);
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
			ComicImageView(QWidget *parent, Size size=Original, const QColor &color=Qt::black);
			~ComicImageView();
			bool onBottom();
			bool onTop();
			QMenu *contextMenu() const;
			Size getSize() const;
			int visiblePages() const;
			const QPixmap image() const;
			int viewWidth() const;
	};
}

#endif

