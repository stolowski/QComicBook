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
#include <QDebug>
#include <algorithm>

using namespace QComicBook;

ContinuousPageView::ContinuousPageView(QWidget *parent, int physicalPages, const ViewProperties& props)
    : PageViewBase(parent, physicalPages, props)
    , m_firstVisible(-1)
    , m_firstVisibleOffset(0)
    , m_y1pos(NULL)
    , m_y2pos(NULL)
{
    //setFocusPolicy(QWidget::StrongFocus);
 
    QWidget *w = new QWidget(this);
    w->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    m_layout = new QVBoxLayout(w);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->setAlignment(Qt::AlignCenter);

    recreatePageWidgets();
    recalculatePageSizes();

    setWidget(w);
    setWidgetResizable(true);
    
    setBackground(props.background());
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    // track scrollbar range changes to restore its relative position
    connect(verticalScrollBar(), SIGNAL(rangeChanged(int, int)), this, SLOT(scrollbarRangeChanged(int, int)));
}

ContinuousPageView::~ContinuousPageView()
{
    delete [] m_y1pos;
    delete [] m_y2pos;
}

void ContinuousPageView::setNumOfPages(int n)
{
    PageViewBase::setNumOfPages(n);
    recreatePageWidgets();
    recalculatePageSizes();
    disposeOrRequestPages();
}

void ContinuousPageView::propsChanged()
{
    qDebug() << "ContinuousPageView::propsChanged()";
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
}

void ContinuousPageView::scrollbarRangeChanged(int min, int max)
{
    qDebug() << "ContinuousPageView::scrollbarRangeChanged";
    //
    // scrollbar ranges were updated (probably due to resizeEvent) -- restore previous page position
    if (m_firstVisible >= 0)
    {
        int offset = m_y1pos[m_firstVisible] + static_cast<int>(m_firstVisibleOffset * (m_y2pos[m_firstVisible]-m_y1pos[m_firstVisible]));
        qDebug() << "scroll offset" << offset;
        verticalScrollBar()->setValue(offset);
    }
}

void ContinuousPageView::recreatePageWidgets()
{
    qDebug() << "ContinuousPageView::recreatePageWidgets()";
    m_firstVisible = -1;
    foreach (PageWidget *p, imgLabel)
    {
        delete p;
    }
    imgLabel.clear();
    delete [] m_y1pos;
    delete [] m_y2pos;
    m_y1pos = m_y2pos = NULL;
    
    int w = viewport()->width() - 10;
    int h = viewport()->height() - 10;

    if (numOfPages())
    {
        if (props.twoPagesMode())
        {
            int i = 0;
            if (numOfPages() % 2) // odd number of pages
            {
                PageWidget *p = new PageWidget(this, w, h, 0);
                imgLabel.append(p);
                m_layout->addWidget(p);
                ++i;
            }
            for (; i<numOfPages(); i+=2)
            {
                PageWidget *p = new PageWidget(this, w, h, i, true);
                imgLabel.append(p);
                m_layout->addWidget(p);
            }
        }
        else
        {
            for (int i=0; i<numOfPages(); i++)
            {
                PageWidget *p = new PageWidget(this, w, h, i);
                p->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                imgLabel.append(p);
                m_layout->addWidget(p);
            }
        }
        
        m_y1pos = new int [imgLabel.size()];
        m_y2pos = new int [imgLabel.size()];
    }
}

PageWidget* ContinuousPageView::findPageWidget(int pageNum) const
{
    if (pageNum >=0 && pageNum < numOfPages())
    {
        if (props.twoPagesMode())
        {
            pageNum = (pageNum + pageNum % 2) / 2;
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
    qDebug() << "ContinuousPageView::disposeOrRequestPages";

    const int vy1 = verticalScrollBar()->value();
    const int vy2 = vy1 + viewport()->height();

    int y = 0;

    m_firstVisible = -1;
    m_firstVisibleOffset = 0.0f;

    for (int i=0; i<imgLabel.size(); i++)
    {
        PageWidget *w = imgLabel[i];
   
        // if page is visible on the screen but not loaded, request it
        if (isInView(m_y1pos[i], m_y2pos[i], vy1, vy2))
        {
            if (w->isDisposed())
            {
                if (!hasRequest(w->pageNumber()))
                {
                    qDebug() << "requesting" << w->pageNumber();
                    addRequest(w->pageNumber(), props.twoPagesMode() && w->hasTwoPages());
                }
            }
            else
            {
                qDebug() << "already loaded" << w->pageNumber();
            }
            if (m_firstVisible < 0)
            {
                m_firstVisible = i;
                m_firstVisibleOffset = static_cast<double>(vy1 - m_y1pos[i]) / w->height(); //visible portion (%) of page
            }
        }
        else // page is not visible
        {
            // if page images are still in memory
            if (!w->isDisposed())
            {
                // dispose page only if none of its neighbours are in view
                if (! ((i>1 && isInView(m_y1pos[i-1], m_y2pos[i-1], vy1, vy2)) || (i<imgLabel.size()-1 && isInView(m_y1pos[i+1], m_y2pos[i+1], vy1, vy2))) )
                {
                    qDebug() << "disposing" << w->pageNumber();
                    w->dispose();
                    delRequest(w->pageNumber(), props.twoPagesMode() && w->hasTwoPages());
                }
            }
            else
            {
                ComicBookSettings &cfg(ComicBookSettings::instance());
                // if previous page is visible then preload this one
                if (i>1 && isInView(m_y1pos[i-1], m_y2pos[i-1], vy1, vy2))
                {
                    if (cfg.preloadPages())
                    {
                        qDebug() << "preloading" << w->pageNumber();
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
    qDebug() << "first visible" << m_firstVisible << "offset" << m_firstVisibleOffset;
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
    qDebug() << "ContinuousPageView::recalculatePageSizes";

    if (imgLabel.size() > 0)
    {
        int avgw = 0;
        int avgh = 0;
        int n = 0;
        foreach (PageWidget *p, imgLabel)
        {
            if (!p->estimatedSize())
            {
                const QSize s(p->size());
                avgw += s.width();
                avgh += s.height();
                ++n;
            }
        }
        if (n > 0)
        {
            int y = 0;
            avgw /= n;
            avgh /= n;
            qDebug() << "estimated w=" << avgw << "h=" << avgh;
           
            for (int i=0; i<imgLabel.size(); i++)
            {
                PageWidget *p = imgLabel[i];
                if (p->estimatedSize())
                {
                    p->setEstimatedSize(avgw, avgh);
                }
                // update positions lookup
                m_y1pos[i] = y;
                m_y2pos[i] = y + p->height();
                y += p->height();
            }
        }
        else
        {
            int y = 0;
            for (int i=0; i<imgLabel.size(); i++)
            {
                PageWidget *p = imgLabel[i];
                // update positions lookup
                m_y1pos[i] = y;
                m_y2pos[i] = y + p->height();
                y += p->height();
            }
        }
    }
}


void ContinuousPageView::scrollContentsBy(int dx, int dy)
{
    qDebug() << "ContinuousPageView::scrollContentsBy y=" << verticalScrollBar()->value();
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
    qDebug() << "setImage:" << img1.getNumber() << "widget page" << w->pageNumber();
    w->setImage(img1);
    recalculatePageSizes();
}

void ContinuousPageView::setImage(const Page &img1, const Page &img2)
{
    Q_ASSERT(numOfPages() > 0);
    delRequest(img1.getNumber(), true, false);

    PageWidget *w = findPageWidget(img1.getNumber());
    Q_ASSERT(w != NULL);
    qDebug() << "setImage:" << img1.getNumber() << img2.getNumber() << "widget page" << w->pageNumber();
    w->setImage(img1, img2);
    recalculatePageSizes();
}

void ContinuousPageView::resizeEvent(QResizeEvent *e)
{
    qDebug() << "ContinuousPageView::resizeEvent"; 
   
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
    PageWidget *w = findPageWidget(n);
    if (w)
    {
        const int idx(imgLabel.indexOf(w));
        Q_ASSERT(idx >= 0);
        m_firstVisibleOffset = 0;
        if (verticalScrollBar()->value() != m_y1pos[idx])
        {
            verticalScrollBar()->setValue(m_y1pos[idx]);
        }
        else
        {
            disposeOrRequestPages();
        }
    }
}

void ContinuousPageView::scrollToTop()
{
    PageWidget *w = currentPageWidget();
    if (w)
    {
        const int idx(imgLabel.indexOf(w));
        Q_ASSERT(idx >= 0);
        verticalScrollBar()->setValue(m_y1pos[idx]);
    }
}

void ContinuousPageView::scrollToBottom()
{
    PageWidget *w = currentPageWidget();
    if (w)
    {
        const int idx(imgLabel.indexOf(w));
        Q_ASSERT(idx >= 0);
        verticalScrollBar()->setValue(m_y2pos[idx] - viewport()->height());
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
            if (isInView(m_y1pos[i], m_y2pos[i], vy1, vy2))
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
        
        const int my1 = std::max(m_y1pos[i], vy1);
        const int my2 = std::min(m_y2pos[i], vy2);
        
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
    qDebug() << "current page" << current;
    return current;
}
