/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __SIMPLEPAGEVIEW_H
#define __SIMPLEPAGEVIEW_H

#include <QList>
#include "Page.h"
#include "PageViewBase.h"

class QVBoxLayout;

namespace QComicBook
{
    class PageWidget;

	class SimplePageView: public PageViewBase
	{
		Q_OBJECT

		protected:
                        void recreatePageWidget();
			virtual void resizeEvent(QResizeEvent *e);
			virtual void wheelEvent(QWheelEvent *e);
                        virtual void scrollContentsBy(int dx, int dy);

                protected slots:
                        void propsChanged();
                        void jobCompleted(const ImageJobResult &result);

		public slots:
			virtual void setImage(const Page &img1);
			virtual void setImage(const Page &img1, const Page &img2);
                        virtual void gotoPage(int n);
                        virtual void scrollToTop();
                        virtual void scrollToBottom();
                        virtual void clear();

		public:
			SimplePageView(QWidget *parent, int physicalPages, const ViewProperties& props);
			virtual ~SimplePageView();

			virtual int visiblePages() const;
			virtual int viewWidth() const;
                        virtual void setNumOfPages(int n);
                        virtual int currentPage() const;

		private:
			int wheelupcnt, wheeldowncnt;
			static const int EXTRA_WHEEL_SPIN; //number of extra wheel spins to flip the page
			static const float JUMP_FACTOR; //factor used to calculate the amount of space to scroll when scrolling page with space
			PageWidget* imgLabel;
                        int m_currentPage;
	};
}

#endif
