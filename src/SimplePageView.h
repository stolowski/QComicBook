#ifndef __SIMPLEPAGEVIEW_H
#define __SIMPLEPAGEVIEW_H

#include <QList>
#include "Page.h"
#include "PageViewBase.h"

namespace QComicBook
{
    class PageWidget;

	class SimplePageView: public PageViewBase
	{
		Q_OBJECT

		protected:
			virtual void resizeEvent(QResizeEvent *e);
			virtual void wheelEvent(QWheelEvent *e);
                        virtual void scrollContentsBy(int dx, int dy);

                protected slots:
                        void propsChanged();

		public slots:
			virtual void setImage(const Page &img1);
			virtual void setImage(const Page &img1, const Page &img2);
                        virtual void setTwoPagesMode(bool f);
                        virtual void scrollToTop();
                        virtual void scrollToBottom();

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

		public:
			SimplePageView(QWidget *parent, int physicalPages, bool twoPagesMode, Size size=Original, const QColor &color=Qt::black);
			virtual ~SimplePageView();

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
			PageWidget* imgLabel;
                        int m_physicalPages;
	};
}

#endif
