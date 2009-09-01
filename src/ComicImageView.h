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
#include <QVector>
#include <QList>
#include <QVBoxLayout>
#include "Page.h"
#include "ViewProperties.h"

class QMenu;
class QPixmap;
class QColor;
class QCursor;

namespace QComicBook
{
    class PageWidget;

	enum Scaling { Smooth, Fast };

	class ComicImageView: public QScrollArea
	{
		Q_OBJECT

		signals:
			void bottomReached();
			void topReached();
			void doubleClick();
                        void requestPage(int);
                        void requestTwoPages(int);
                        void cancelPageRequest(int);
                        void cancelTwoPagesRequest(int);

		protected:
			void resizeEvent(QResizeEvent *e);
			virtual void contextMenuEvent(QContextMenuEvent *e);
			virtual void wheelEvent(QWheelEvent *e);
			virtual void mouseMoveEvent(QMouseEvent *e);
			virtual void mousePressEvent(QMouseEvent *e);
			virtual void mouseReleaseEvent(QMouseEvent *e);
			virtual void mouseDoubleClickEvent(QMouseEvent *e);
                        virtual void scrollContentsBy(int dx, int dy);
			void scrollByDelta(int dx, int dy);
                        void recreatePageWidgets();
                        PageWidget *findPageWidget(int pageNum) const;
                        void recalculatePageSizes();
                        QList<PageWidget *> findPageWidgetsInView() const;
                        void disposeUnneededPageWidgets();
                        void disposeOrRequestPages();

                protected slots:
                        void propsChanged();

		public slots:
			void setImage(const Page &img1);
			void setImage(const Page &img1, const Page &img2);
			void setRotation(Rotation r);
                        void setTwoPagesMode(bool f);
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
			ComicImageView(QWidget *parent, int physicalPages, bool twoPagesMode, Size size=Original, const QColor &color=Qt::black);
			virtual ~ComicImageView();
			bool onBottom();
			bool onTop();
			QMenu *contextMenu() const;
                        Size getSize() const;
			int visiblePages() const;
			const QPixmap image() const;
			int viewWidth() const;
                        ViewProperties& properties();
                        void setNumOfPages(int n);

		private:
			QMenu *context_menu;
			ViewProperties props;
			int spdx, spdy; //scroll speed
			int lx, ly; //last mouse position when tracking mouse movements
			int wheelupcnt, wheeldowncnt;
			QCursor *smallcursor;
			static const int EXTRA_WHEEL_SPIN; //number of extra wheel spins to flip the page
			static const float JUMP_FACTOR; //factor used to calculate the amount of space to scroll when scrolling page with space
			QVector<PageWidget*> imgLabel;
                        QVBoxLayout *m_layout;
                        int m_physicalPages;
                        bool m_twoPagesMode;
                        bool m_contMode; //continuous mode flag
                        QList<int> m_requestedPages;
	};
}

#endif

