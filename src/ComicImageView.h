/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005 Pawel Stolowski <pawel.stolowski@wp.pl>
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
			int lx, ly; //last mouse position when tracking mouse movements
			int wheelupcnt, wheeldowncnt;
			QCursor *smallcursor;
                        QColor background; //background color
                        bool pagenumbers; //if page numbers should be embedded
			static const int EXTRA_WHEEL_SPIN; //number of extra wheel spins to flip the page
			static const float JUMP_FACTOR; //factor used to calculate the amount of space to scroll when scrolling page with space
			QLabel *imgLabel;
			int totalWidth;
			int totalHeight;

		signals:
			void bottomReached();
			void topReached();
			void doubleClick();

		protected:
			void resizeEvent(QResizeEvent *e);
			void updateImageSize();
			void redrawImages();
			virtual void contextMenuEvent(QContextMenuEvent *e);
			virtual void wheelEvent(QWheelEvent *e);
			virtual void mouseMoveEvent(QMouseEvent *e);
			virtual void mousePressEvent(QMouseEvent *e);
			virtual void mouseReleaseEvent(QMouseEvent *e);
			virtual void mouseDoubleClickEvent(QMouseEvent *e);
			void scrollByDelta(int dx, int dy);
                        void drawPageNumber(int page, QPainter &p, int x, int y);

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
                        void showPageNumbers(bool f);

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

