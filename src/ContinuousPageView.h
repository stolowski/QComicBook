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
                        PageWidget *currentPageWidget() const;

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
                        virtual void clear();
                        virtual void gotoPage(int n);
                        virtual void scrollToTop();
                        virtual void scrollToBottom();

		public:
			ContinuousPageView(QWidget *parent, int physicalPages, const ViewProperties& props);
			virtual ~ContinuousPageView();

			virtual int visiblePages() const;
			virtual int viewWidth() const;
                        virtual void setNumOfPages(int n);
                        virtual int currentPage() const;

		private:
			QVector<PageWidget*> imgLabel;
                        int *m_y1pos;
                        int *m_y2pos;
                        int m_firstVisible; //first visible page in the view
                        double m_firstVisibleOffset; //visible portion (%) of first visible page
                        QVBoxLayout *m_layout;
	};
}

#endif

