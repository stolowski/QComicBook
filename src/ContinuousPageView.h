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

#ifndef __CONTINUOUSPAGEVIEW_H
#define __CONTINUOUSPAGEVIEW_H

#include <QVector>
#include <QList>
#include <QVBoxLayout>
#include "Page.h"
#include "PageViewBase.h"

namespace QComicBook
{
    class PageWidget;

	class ContinuousPageView: public PageViewBase
	{
		Q_OBJECT

		protected:
			virtual void resizeEvent(QResizeEvent *e);
			virtual void wheelEvent(QWheelEvent *e);
                        virtual void scrollContentsBy(int dx, int dy);

                        void recreatePageWidgets();
                        PageWidget *findPageWidget(int pageNum) const;
                        void recalculatePageSizes();
                        QList<PageWidget *> findPageWidgetsInView() const;
                        void disposeOrRequestPages();

                protected slots:
                        void propsChanged();
                        void scrollbarRangeChanged(int min, int max);
                        static bool isInView(int y1, int y2, int vy1, int vy2)
                        {
                            return std::min(y2, vy2) > std::max(y1, vy1);
                        }

		public slots:
			virtual void setImage(const Page &img1);
			virtual void setImage(const Page &img1, const Page &img2);
                        virtual void setTwoPagesMode(bool f);

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
                        virtual void clear();
                        virtual void gotoPage(int n);

		public:
			ContinuousPageView(QWidget *parent, int physicalPages, bool twoPagesMode, Size size=Original, const QColor &color=Qt::black);
			virtual ~ContinuousPageView();

			virtual int visiblePages() const;
			virtual const QPixmap image() const;
			virtual int viewWidth() const;
                        virtual void setNumOfPages(int n);
                        virtual int currentPage() const;

		private:
			int spdx, spdy; //scroll speed
			int wheelupcnt, wheeldowncnt;
			static const int EXTRA_WHEEL_SPIN; //number of extra wheel spins to flip the page
			static const float JUMP_FACTOR; //factor used to calculate the amount of space to scroll when scrolling page with space
			QVector<PageWidget*> imgLabel;
                        int *m_y1pos;
                        int *m_y2pos;
                        int m_firstVisible; //first visible page in the view
                        double m_firstVisibleOffset; //visible portion (%) of first visible page
                        QVBoxLayout *m_layout;
                        int m_physicalPages;
	};
}

#endif

