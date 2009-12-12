#ifndef __PAGEVIEWBASE_H
#define __PAGEVIEWBASE_H

#include <QGraphicsView>
#include "ViewProperties.h"

class QMenu;
class QGraphicsScene;

namespace QComicBook
{
    class PageWidget;
    class Page;

	enum Scaling { Smooth, Fast };

	class PageViewBase: public QGraphicsView
	{
	Q_OBJECT

        signals:
            void currentPageChanged(int);
            void bottomReached();
            void topReached();
            void doubleClick();
            void requestPage(int);
            void requestTwoPages(int);
            void cancelPageRequest(int);
            void cancelTwoPagesRequest(int);

        public slots:
            virtual void setImage(const Page &img1) = 0;
            virtual void setImage(const Page &img1, const Page &img2) = 0;
            virtual void clear() = 0;
            virtual void gotoPage(int n) = 0;
            virtual void scrollToTop() = 0;
            virtual void scrollToBottom() = 0;
            virtual void jumpUp();
            virtual void jumpDown();

            virtual void setTwoPagesMode(bool f);
            virtual void setMangaMode(bool f);
            virtual void setBackground(const QColor &color);
            virtual void setSmallCursor(bool f);
            virtual void showPageNumbers(bool f);
            virtual void setRotation(Rotation r);
            virtual void rotateRight();
            virtual void rotateLeft();
            virtual void resetRotation();
            virtual void setSize(Size s);
            virtual void scrollUp();
            virtual void scrollDown();
            virtual void scrollUpFast();
            virtual void scrollDownFast();
            virtual void scrollRight();
            virtual void scrollLeft();
            virtual void scrollRightFast();
            virtual void scrollLeftFast();
            virtual void propsChanged() = 0;

        public:
            PageViewBase(QWidget *parent, int physicalPages, const ViewProperties &props);
            virtual ~PageViewBase();

            void enableScrollbars(bool f);
            QMenu *contextMenu() const;
            ViewProperties& properties();
            bool onBottom();
            bool onTop();

            virtual void setNumOfPages(int n);
            int numOfPages() const;
            virtual int visiblePages() const = 0;
            virtual int viewWidth() const = 0;
            virtual int currentPage() const = 0;
            virtual int nextPage(int page) const;
            virtual int previousPage(int page) const;
            virtual int roundPageNumber(int page) const;

        protected:
            virtual void resizeEvent(QResizeEvent *e);
            virtual void contextMenuEvent(QContextMenuEvent *e);
            virtual void mouseMoveEvent(QMouseEvent *e);
            virtual void mousePressEvent(QMouseEvent *e);
            virtual void mouseReleaseEvent(QMouseEvent *e);
            virtual void mouseDoubleClickEvent(QMouseEvent *e);
            void scrollByDelta(int dx, int dy);
            void recalculateScrollSpeeds();

            bool hasRequest(int page) const;
            void addRequest(int page, bool twoPages);
            void delRequest(int page, bool twoPages, bool cancel=true);
            void delRequests();

            ViewProperties props;
            QGraphicsScene *scene;

        private:
            static const float JUMP_FACTOR; //factor used to calculate the amount of space to scroll when scrolling page with space
            QMenu *context_menu;
            int m_physicalPages;
            int lx, ly; //last mouse position when tracking mouse movements
            int spdx, spdy; //scroll speed
            int wheelupcnt, wheeldowncnt;
            QCursor *smallcursor;
            QList<int> m_requestedPages;
        };
}

#endif
