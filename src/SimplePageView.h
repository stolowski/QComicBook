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

		public slots:
			virtual void setImage(const Page &img1);
			virtual void setImage(const Page &img1, const Page &img2);
                        virtual void gotoPage(int n);
                        virtual void setTwoPagesMode(bool f);
                        virtual void scrollToTop();
                        virtual void scrollToBottom();

			void jumpUp();
			void jumpDown();
                        virtual void clear();

		public:
			SimplePageView(QWidget *parent, PageLoaderThread *loader, int physicalPages, const ViewProperties& props);
			virtual ~SimplePageView();

			virtual int visiblePages() const;
			virtual int viewWidth() const;
                        virtual void setNumOfPages(int n);
                        virtual int currentPage() const;

		private:
			int wheelupcnt, wheeldowncnt;
			static const int EXTRA_WHEEL_SPIN; //number of extra wheel spins to flip the page
			static const float JUMP_FACTOR; //factor used to calculate the amount of space to scroll when scrolling page with space
                        QVBoxLayout *m_layout;
			PageWidget* imgLabel;
                        int m_currentPage;
	};
}

#endif
