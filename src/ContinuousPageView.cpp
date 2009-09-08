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
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QtGlobal>
#include <QResizeEvent>
#include <QScrollBar>
#include <QDebug>
#include <algorithm>

using namespace QComicBook;

const int ContinuousPageView::EXTRA_WHEEL_SPIN = 3;
const float ContinuousPageView::JUMP_FACTOR = 0.85f;

ContinuousPageView::ContinuousPageView(QWidget *parent, int physicalPages, bool twoPagesMode, Size size, const QColor &color)
    : PageViewBase(parent, twoPagesMode, size, color)
    , wheelupcnt(0), wheeldowncnt(0)
    , m_physicalPages(physicalPages)
    , m_firstVisible(-1)
    , m_firstVisibleOffset(0)
{
    //setFocusPolicy(QWidget::StrongFocus);
 
    QWidget *w = new QWidget(this);
    w->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    m_layout = new QVBoxLayout(w);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setAlignment(Qt::AlignCenter);
    recreatePageWidgets();
    setWidget(w);
    setWidgetResizable(true);
    
    setBackground(color);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

ContinuousPageView::~ContinuousPageView()
{
}

void ContinuousPageView::setNumOfPages(int n)
{
    m_physicalPages = n;
    recreatePageWidgets();
    disposeOrRequestPages();
}

void ContinuousPageView::propsChanged()
{
    foreach (PageWidget *p, imgLabel)
    {
        p->redrawImages();
    }
    disposeOrRequestPages();
    update();
}

void ContinuousPageView::recreatePageWidgets()
{
    m_firstVisible = -1;
    foreach (PageWidget *p, imgLabel)
    {
        delete p;
    }
    imgLabel.clear();

    if (props.twoPagesMode())
    {
        int i = 0;
        if (m_physicalPages % 2) // odd number of pages
        {
            PageWidget *p = new PageWidget(this, 0);
            imgLabel.append(p);
            m_layout->addWidget(p);
            ++i;
        }
        for (; i<m_physicalPages; i+=2)
        {
            PageWidget *p = new PageWidget(this, i, true);
            imgLabel.append(p);
            m_layout->addWidget(p);
        }
    }
    else
    {
        for (int i=0; i<m_physicalPages; i++)
        {
            PageWidget *p = new PageWidget(this, i);
            p->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            imgLabel.append(p);
            m_layout->addWidget(p);
        }
    }
}

PageWidget* ContinuousPageView::findPageWidget(int pageNum) const
{
    if (props.twoPagesMode())
    {
        pageNum = (pageNum + pageNum % 2) / 2;
    }
    if (pageNum < imgLabel.size())
    {
        return imgLabel[pageNum];
    }
    return NULL;
}

void ContinuousPageView::disposeOrRequestPages()
{
    const int vy1 = verticalScrollBar()->value();
    const int vy2 = vy1 + viewport()->height();

    m_firstVisible = -1;
    m_firstVisibleOffset = 0;

    for (int i=0; i<imgLabel.size(); i++)
    {
        PageWidget *w = imgLabel[i];
        if (w->isInView(vy1, vy2))
        {
            if (w->estimatedSize() || w->isDisposed())
            {
                if (!hasRequest(w->pageNumber()))
                {
                    addRequest(w->pageNumber(), props.twoPagesMode() && w->hasTwoPages());
                }
            }
            if (m_firstVisible < 0)
            {
                m_firstVisible = i;
                m_firstVisibleOffset = vy1 - w->pos().y();
            }
        }
        else
        {
            if (!w->estimatedSize() && !w->isDisposed())
            {
                // dispose page only if none of its neighbours are in view
                if (! ((i>1 && imgLabel[i-1]->isInView(vy1, vy2)) || (i<imgLabel.size()-1 && imgLabel[i+1]->isInView(vy1, vy2))) )
                {
                    w->dispose();
                }
            }
            delRequest(w->pageNumber(), props.twoPagesMode() && w->hasTwoPages());
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
            avgw /= n;
            avgh /= n;
            qDebug() << "estimated w=" << avgw << "h=" << avgh;
            foreach (PageWidget *p, imgLabel)
            {
                if (p->estimatedSize())
                {
                    p->setEstimatedSize(avgw, avgh);
                }
            }
        }
    }
}


void ContinuousPageView::scrollContentsBy(int dx, int dy)
{
    PageViewBase::scrollContentsBy(dx, dy);
    qDebug() << "ContinuousPageView::scrollContentsBy y=" << verticalScrollBar()->value();
    disposeOrRequestPages();
    
    const int n = currentPage();
    if (n>=0)
    {
        emit currentPageChanged(n);
    }
}

void ContinuousPageView::setImage(const Page &img1)
{
    Q_ASSERT(m_physicalPages > 0);
    //if (!preserveangle)
    //          iangle = 0;
    PageWidget *w = findPageWidget(img1.getNumber());
    Q_ASSERT(w != NULL);
    w->setImage(img1);
    recalculatePageSizes();
}

void ContinuousPageView::setImage(const Page &img1, const Page &img2)
{
    Q_ASSERT(m_physicalPages > 0);
    //  if (!preserveangle)
    //          iangle = 0;
    PageWidget *w = findPageWidget(img1.getNumber());
    Q_ASSERT(w != NULL);
    qDebug() << "setImage:" << img1.getNumber() << img2.getNumber() << "widget page" << w->pageNumber();
    w->setImage(img1, img2);
    recalculatePageSizes();
}

void ContinuousPageView::resizeEvent(QResizeEvent *e)
{
    qDebug() << "ContinuousPageView::resizeEvent";
    
    if (m_firstVisible >= 0)
    {
        qDebug() << "first visible y=" << imgLabel[m_firstVisible]->pos().y() << "h=" << imgLabel[m_firstVisible]->height();
    }

    foreach (PageWidget *w, imgLabel)
    {
        w->redrawImages();
    }
    recalculatePageSizes();
    //viewport()->updateGeometry();
    // disposeOrRequestPages();

    if (m_firstVisible >= 0)
    {
        int offset = 0;
        for (int i=0; i<m_firstVisible; i++)
        {
            offset += imgLabel[i]->height();
        }
        qDebug() << "after upd first visible y=" << imgLabel[m_firstVisible]->pos().y() << "h=" << imgLabel[m_firstVisible]->height() << "should be" << offset;

        const int newPos = imgLabel[m_firstVisible]->pos().y();// + m_firstVisibleOffset;

        if (verticalScrollBar()->value() != newPos)
        {
            qDebug() << "old Y pos=" << verticalScrollBar()->value() << "new=" << newPos;
            
        }
        verticalScrollBar()->setValue(offset);
//        ensureVisible(0, offset, 0, 0);
    }
   
    //viewport()->update();
//    e->accept();
    QScrollArea::resizeEvent(e);
}

void ContinuousPageView::wheelEvent(QWheelEvent *e)
{
    e->accept();
//TODO
    /*e->accept();
        if (e->delta() > 0) //scrolling up
        {
                if (imgLabel->height() <= height() || (onTop() && ++wheelupcnt > EXTRA_WHEEL_SPIN))
                {
                        wheelupcnt = 0;
                        emit topReached();
                }
                else
                {
                        scrollByDelta(0, -3*spdy);
                        wheeldowncnt = 0; //reset opposite direction counter
                }
        }
        else //scrolling down
        {
                if (imgLabel->height() <= height() || (onBottom() && ++wheeldowncnt > EXTRA_WHEEL_SPIN))
                {
                        wheeldowncnt = 0;
                        emit bottomReached();
                }
                else
                {
                        scrollByDelta(0, 3*spdy);
                        wheelupcnt = 0; //reset opposite direction counter
                }
                }*/
}


void ContinuousPageView::setTwoPagesMode(bool f)
{
    /*if (f != m_twoPagesMode)
    {
        m_twoPagesMode = f;
        recreatePageWidgets();
        }*/
    props.setTwoPagesMode(f);
    recreatePageWidgets(); //FIXME
}

void ContinuousPageView::scrollRight()
{
        scrollByDelta(spdx, 0);
}

void ContinuousPageView::scrollLeft()
{
        scrollByDelta(-spdx, 0);
}

void ContinuousPageView::scrollRightFast()
{
        scrollByDelta(3*spdx, 0);
}

void ContinuousPageView::scrollLeftFast()
{
        scrollByDelta(-3*spdx, 0);
}

void ContinuousPageView::scrollUp()
{
        if (onTop())
        {
                wheelupcnt = wheeldowncnt = 0;
                emit topReached();
        }
        else
		scrollByDelta(0, -spdy);
}

void ContinuousPageView::scrollDown()
{
        if (onBottom())
        {
                wheelupcnt = wheeldowncnt = 0;
                emit bottomReached();
        }
        else
		scrollByDelta(0, spdy);
}

void ContinuousPageView::scrollUpFast()
{
        if (onTop())
                emit topReached();
        else
		scrollByDelta(0, -3*spdy);
}

void ContinuousPageView::scrollDownFast()
{       
        if (onBottom())
                emit bottomReached();
        else
		scrollByDelta(0, 3*spdy);
}

void ContinuousPageView::rotateRight()
{
        setRotation(QComicBook::Right);
}

void ContinuousPageView::rotateLeft()
{
        setRotation(QComicBook::Left);
}

void ContinuousPageView::resetRotation()
{
        setRotation(None);
}

void ContinuousPageView::jumpUp()
{
        if (onTop())
                emit topReached();
        else 
		scrollByDelta(0, -static_cast<int>(JUMP_FACTOR * viewport()->height()));
}

void ContinuousPageView::jumpDown()
{
        if (onBottom())
                emit bottomReached();
        else 
		scrollByDelta(0, static_cast<int>(JUMP_FACTOR * viewport()->height()));
}

void ContinuousPageView::gotoPage(int n)
{
    PageWidget *w = findPageWidget(n);
    if (w)
    {
        scrollContentsBy(0, w->pos().y());
    }
}

void ContinuousPageView::clear()
{
    m_physicalPages = 0;
    m_firstVisible = -1;
    delRequests();
    recreatePageWidgets();
}
			
int ContinuousPageView::visiblePages() const
{
    return props.twoPagesMode() ? 2 : 1;
    //return imgLabel->numOfPages();
}

const QPixmap ContinuousPageView::image() const
{
//TODO
//    if (imgLabel->numOfPages())
//        return QPixmap(*imgLabel->pixmap());
    return QPixmap(); //fallback
}

int ContinuousPageView::viewWidth() const
{
    return width(); //TODO?
//    return (imgLabel->numOfPages()) ? imgLabel->width() : 0;
}

int ContinuousPageView::currentPage() const
{
    double maxv(0.0f);
    int current(-1);
    const int vy1 = verticalScrollBar()->value();
    const int vy2 = vy1 + viewport()->height();
    //
    // find page with most part displayed on the view
    int i = 0;
    if (m_firstVisible < 0)
    {
        for (; i<imgLabel.size(); i++)
        {
            if (imgLabel[i]->isInView(vy1, vy2))
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
        
        const int y1 = w->pos().y();
        const int y2 = y1 + w->height();

        const int my1 = std::max(y1, vy1);
        const int my2 = std::min(y2, vy2);
        
        if (my2 <= my1)
        {
            break;
        }
        // percentage of page displayed
        const double v = (100.0f * static_cast<double>(my2 - my1)) / static_cast<double>(w->height());
        if (v > maxv)
        {
            maxv = v;
            current = w->pageNumber();
        }
        ++i;
    }
    qDebug() << "current page" << current;
    return current;
}
