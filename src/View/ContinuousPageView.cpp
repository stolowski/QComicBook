/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2011 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "ContinuousPageView.h"
#include "Utility.h"
#include "PageWidget.h"
#include "ComicBookSettings.h"
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QtGlobal>
#include <QResizeEvent>
#include <QScrollBar>
#include <algorithm>
#include "../ComicBookDebug.h"

using namespace QComicBook;

ContinuousPageView::ContinuousPageView(QWidget *parent, int physicalPages, const ViewProperties& props)
    : PageViewBase(parent, physicalPages, props)
    , m_firstVisible(-1)
    , m_firstVisibleOffset(0)
    , m_requestedPage(-1)
{
    _DEBUG;

    recreatePageWidgets();
    recalculatePageSizes();

    setBackground(props.background());
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    // track scrollbar range changes to restore its relative position
    connect(verticalScrollBar(), SIGNAL(rangeChanged(int, int)), this, SLOT(scrollbarRangeChanged(int, int)));
}

ContinuousPageView::~ContinuousPageView()
{
    _DEBUG;
}

void ContinuousPageView::setNumOfPages(int n)
{
    _DEBUG << n;
    PageViewBase::setNumOfPages(n);
    recreatePageWidgets();
    recalculatePageSizes();
    disposeOrRequestPages();
}

void ContinuousPageView::propsChanged()
{
    _DEBUG;
    if ((props.twoPagesMode() && imgLabel.size() == numOfPages()) || (!props.twoPagesMode() && imgLabel.size() < numOfPages()))
    {
        recreatePageWidgets();
    }
    foreach (PageWidget *p, imgLabel)
    {
        p->redrawImages();
    }
    recalculatePageSizes();
    disposeOrRequestPages();
    update();

    updateSceneRect();
}

void ContinuousPageView::jobCompleted(const ImageJobResult &result)
{
    _DEBUG;
    foreach (PageWidget *p, imgLabel)
    {
        if (p->jobCompleted(result))
        {
            updateSceneRect();
            break;
        }
    }
}

void ContinuousPageView::scrollbarRangeChanged(int min, int max)
{
    _DEBUG;
    //
    // scrollbar ranges were updated (probably due to resizeEvent) -- restore previous page position
    if (m_firstVisible >= 0)
    {
        int offset = m_ypos.startCoordinate(m_firstVisible) + static_cast<int>(m_firstVisibleOffset * (m_ypos.endCoordinate(m_firstVisible)-m_ypos.startCoordinate(m_firstVisible)));
        _DEBUG << "scroll offset" << offset;
        verticalScrollBar()->setValue(offset);
    }
}

void ContinuousPageView::recreatePageWidgets()
{
    _DEBUG;
    m_firstVisible = -1;
    foreach (PageWidget *p, imgLabel)
    {
        delete p;
    }
    imgLabel.clear();
    m_ypos.reset();
    
    int w = viewport()->width() - 10;
    int h = viewport()->height() - 10;

    _DEBUG << "viewport w" << w << "h" << h;

    if (numOfPages())
    {
        if (props.twoPagesMode())
        {
            int i = 0;
            for (; i<roundPageNumber(numOfPages()); i+=2)
            {
                PageWidget *p = new PageWidget(this, w, h, i, true);
		_DEBUG << "creating PageWidget for two pages" << i;
                imgLabel.append(p);
                scene->addItem(p);
            }
            if (numOfPages() & 1) // odd number of pages
            {
                PageWidget *p = new PageWidget(this, w, h, i);
		_DEBUG << "creating PageWidget for one page" << i;
                imgLabel.append(p);
                scene->addItem(p);
            }

        }
        else
        {
            for (int i=0; i<numOfPages(); i++)
            {
                PageWidget *p = new PageWidget(this, w, h, i);
		_DEBUG << "creating PageWidget for one page" << i;
                imgLabel.append(p);
                scene->addItem(p);
            }
        }
        
        m_ypos.resize(imgLabel.size());
    }
}

PageWidget* ContinuousPageView::findPageWidget(int pageNum) const
{
    if (pageNum >=0 && pageNum < numOfPages())
    {
        if (props.twoPagesMode())
        {
            pageNum /= 2;
        }
        if (pageNum < imgLabel.size())
        {
            return imgLabel[pageNum];
        }
    }
    return NULL;
}

void ContinuousPageView::disposeOrRequestPages()
{
    _DEBUG;

    const int vy1 = verticalScrollBar()->value();
    const int vy2 = vy1 + viewport()->height();

    int y = 0;

    m_firstVisible = -1;
    m_firstVisibleOffset = 0.0f;

    _DEBUG << "view rect:" << vy1 << "," << vy2;

    for (int i=0; i<imgLabel.size(); i++)
    {
        PageWidget *w = imgLabel[i];

        _DEBUG << "page widget:" << i << " page num:" << w->pageNumber() << " y=" << w->y() << " ypos=" << m_ypos.startCoordinate(i);
   
        // if page is visible on the screen but not loaded, request it
        if (isInView(m_ypos.startCoordinate(i), m_ypos.endCoordinate(i), vy1, vy2))
        {
            _DEBUG << "in view:" << w->pageNumber();
            if (w->isDisposed())
            {
                if (!hasRequest(w->pageNumber()))
                {
                    _DEBUG << "requesting" << w->pageNumber();
                    addRequest(w->pageNumber(), props.twoPagesMode() && w->hasTwoPages());
                }
            }
            else
            {
                _DEBUG << "already loaded" << w->pageNumber();
                emit w->hasTwoPages() ? pageReady(w->getPage(0), w->getPage(1)) : pageReady(w->getPage(0));
            }
            if (m_firstVisible < 0)
            {
                m_firstVisible = i;
                m_firstVisibleOffset = static_cast<double>(vy1 - m_ypos.startCoordinate(i)) / w->height(); //visible portion (%) of page
            }
        }
        else // page is not visible
        {
            // if page images are still in memory
            if (!w->isDisposed())
            {
                _DEBUG << "not in view & not disposed:" << w->pageNumber();
                // dispose page only if none of its neighbours are in view
                if (! ((i>1 && isInView(m_ypos.startCoordinate(i-1), m_ypos.endCoordinate(i-1), vy1, vy2)) || (i<imgLabel.size()-1 && isInView(m_ypos.startCoordinate(i+1), m_ypos.endCoordinate(i+1), vy1, vy2))) )
                {
                    _DEBUG << "disposing" << w->pageNumber();
                    w->dispose();
                    delRequest(w->pageNumber(), props.twoPagesMode() && w->hasTwoPages());
                }
            }
            else
            {
                _DEBUG << "not in view & disposed:" << w->pageNumber();
                ComicBookSettings &cfg(ComicBookSettings::instance());
                // if previous page is visible then preload this one
                if (i>0 && isInView(m_ypos.startCoordinate(i-1), m_ypos.endCoordinate(i-1), vy1, vy2))
                {
                    _DEBUG << "previous page visible";
                    if (cfg.preloadPages())
                    {
                        _DEBUG << "preloading" << w->pageNumber();
                        addRequest(w->pageNumber(), props.twoPagesMode() && w->hasTwoPages());
                    }
                }
                else
                {
                    delRequest(w->pageNumber(), props.twoPagesMode() && w->hasTwoPages());                   
                }
            }
        }
    }
    _DEBUG << "first visible" << m_firstVisible << "offset" << m_firstVisibleOffset;
}

QList<PageWidget *> ContinuousPageView::findPageWidgetsInView() const
{
    const int vy1 = verticalScrollBar()->value();
    const int vy2 = vy1 + viewport()->height();

    QList<PageWidget *> wlist;
    int i;
    for (i=0; i<imgLabel.size(); i++)
    {
        if (imgLabel[i]->isInView(vy1, vy2))
        {
            break;
        }
    }
    while (i < imgLabel.size() && imgLabel[i]->isInView(vy1, vy2)) 
    {
        wlist.append(imgLabel[i]);
        ++i;
    }
    return wlist;
}

void ContinuousPageView::recalculatePageSizes()
{
    _DEBUG;

    if (imgLabel.size() > 0)
    {
        int avgw = 0;
        int avgh = 0;
        int n = 0;
        foreach (PageWidget *p, imgLabel)
        {
            if (!p->isEstimated())
            {
                const QSize s(p->estimatedSize());
                avgw += s.width();
                avgh += s.height();
                ++n;
            }
        }
        int y = 0;
	if (n > 0) //if we had at least one real size, calculate and set estimated sizes
	{
		avgw /= n;
		avgh /= n;
		_DEBUG << "estimated w=" << avgw << "h=" << avgh;
	}

	for (int i=0; i<imgLabel.size(); i++)
	{
		PageWidget *p = imgLabel[i];
		if (n > 0 && p->isEstimated())
		{
			p->setEstimatedSize(avgw, avgh);
		}
		p->setPos(0, y);
                center(p, true, false); // center horizontally only, preserve Y
                _DEBUG << "page: " << i << ", y=" << y;
		// update positions lookup
		m_ypos.startCoordinate(i) = y;
		m_ypos.endCoordinate(i) = y + p->estimatedSize().height();
		y += p->estimatedSize().height();
	}
    }
    updateSceneRect();
}


void ContinuousPageView::scrollContentsBy(int dx, int dy)
{
    _DEBUG << "ContinuousPageView::scrollContentsBy y=" << verticalScrollBar()->value();
    PageViewBase::scrollContentsBy(dx, dy);
    disposeOrRequestPages();
    
    const int n = currentPage();
    if (n>=0)
    {
        emit currentPageChanged(n);
    }
}

void ContinuousPageView::setImage(const Page &img1)
{
    Q_ASSERT(numOfPages() > 0);
    delRequest(img1.getNumber(), false, false);

    PageWidget *w = findPageWidget(img1.getNumber());
    Q_ASSERT(w != NULL);
    _DEBUG << img1.getNumber();
    w->setImage(img1);
    recalculatePageSizes();

    if (m_requestedPage >= 0) {
        _DEBUG << "requested page =" << m_requestedPage << ", scrolling";
        verticalScrollBar()->setValue(w->y());
        
        m_firstVisible = imgLabel.indexOf(findPageWidget(m_requestedPage));
        m_firstVisibleOffset = 0;
        m_requestedPage = -1;
    }
    emit pageReady(img1);
}

void ContinuousPageView::setImage(const Page &img1, const Page &img2)
{
    Q_ASSERT(numOfPages() > 0);
    delRequest(img1.getNumber(), true, false);

    PageWidget *w = findPageWidget(img1.getNumber());
    Q_ASSERT(w != NULL);
    _DEBUG << img1.getNumber() << img2.getNumber() << "widget page" << w->pageNumber();
    w->setImage(img1, img2);
    recalculatePageSizes();

    if (m_requestedPage >= 0) {
        _DEBUG << "requested page =" << m_requestedPage << ", scrolling";
        verticalScrollBar()->setValue(w->y());
        m_firstVisible =  imgLabel.indexOf(findPageWidget(m_requestedPage));
        m_firstVisibleOffset = 0;
        m_requestedPage = -1;
    }
    emit pageReady(img1, img2);
}

void ContinuousPageView::resizeEvent(QResizeEvent *e)
{
    _DEBUG;
   
    foreach (PageWidget *w, imgLabel)
    {
        w->redrawImages();
    }

    recalculatePageSizes();
    /*if (e->oldSize().height() != e->size().height())
    {
        disposeOrRequestPages();
    }*/
    PageViewBase::resizeEvent(e);
}

void ContinuousPageView::wheelEvent(QWheelEvent *e)
{
    PageViewBase::wheelEvent(e);
}

void ContinuousPageView::gotoPage(int n)
{
    _DEBUG << n;
    PageWidget *w = findPageWidget(n);
    if (w)
    {
        const int idx(imgLabel.indexOf(w));
        Q_ASSERT(idx >= 0);
        m_firstVisibleOffset = 0;
        if (verticalScrollBar()->value() != m_ypos.startCoordinate(idx))
        {
            verticalScrollBar()->setValue(m_ypos.startCoordinate(idx));
        }
        //
        // set m_requestedPage only if page is not loaded already,
        // otherwise it won't be requested and setImage will scroll
        // to wrong page.
        if (w->isDisposed())
        {
            m_requestedPage = n;
        }
        disposeOrRequestPages();
        emit currentPageChanged(roundPageNumber(n));
    }
}

void ContinuousPageView::scrollToTop()
{
    PageWidget *w = currentPageWidget();
    if (w)
    {
        const int idx(imgLabel.indexOf(w));
        Q_ASSERT(idx >= 0);
        verticalScrollBar()->setValue(m_ypos.startCoordinate(idx));
    }
}

void ContinuousPageView::scrollToBottom()
{
    PageWidget *w = currentPageWidget();
    if (w)
    {
        const int idx(imgLabel.indexOf(w));
        Q_ASSERT(idx >= 0);
        verticalScrollBar()->setValue(m_ypos.endCoordinate(idx) - viewport()->height());
    }
}

void ContinuousPageView::clear()
{
    m_firstVisible = -1;
    delRequests();
    setNumOfPages(0);
}
			
int ContinuousPageView::visiblePages() const
{
    PageWidget *w(currentPageWidget());
    return w ? w->numOfPages() : 0;
}

int ContinuousPageView::viewWidth() const
{
    return width(); //TODO?
//    return (imgLabel->numOfPages()) ? imgLabel->width() : 0;
}

PageWidget *ContinuousPageView::currentPageWidget() const
{
    double maxv(0.0f);
    PageWidget *current(NULL);
    const int vy1 = verticalScrollBar()->value();
    const int vy2 = vy1 + viewport()->height();
    //
    // find page with most part displayed on the view
    int i = 0;
    if (m_firstVisible < 0)
    {
        for (; i<imgLabel.size(); i++)
        {
            if (isInView(m_ypos.startCoordinate(i), m_ypos.endCoordinate(i), vy1, vy2))
            {
                break;
            }
        }
    }
    else
    {
        i = m_firstVisible;
    }
    while (i < imgLabel.size())
    {
        PageWidget *w = imgLabel[i];
        
        const int my1 = std::max(m_ypos.startCoordinate(i), vy1);
        const int my2 = std::min(m_ypos.endCoordinate(i), vy2);
        
        if (my2 <= my1)
        {
            break;
        }
        // percentage of page displayed
        const double v = (100.0f * static_cast<double>(my2 - my1)) / static_cast<double>(w->height());
        if (v > maxv)
        {
            maxv = v;
            current = w;
        }
        ++i;
    }
    return current;
}

int ContinuousPageView::currentPage() const
{
    int current(-1);
    const PageWidget *w(currentPageWidget());
    if (w)
    {
        current = w->pageNumber();
    }
    _DEBUG << "current page" << current;
    return current;
}
