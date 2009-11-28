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

#include "SimplePageView.h"
#include "Utility.h"
#include "PageWidget.h"
#include "ComicBookSettings.h"
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QtGlobal>
#include <QResizeEvent>
#include <QScrollBar>
#include <QDebug>

using namespace QComicBook;

const int SimplePageView::EXTRA_WHEEL_SPIN = 3;

SimplePageView::SimplePageView(QWidget *parent, int physicalPages, const ViewProperties& props)
    : PageViewBase(parent, physicalPages, props)
    , wheelupcnt(0), wheeldowncnt(0)
    , m_currentPage(0) //??
    , imgLabel(NULL)
{
    //setFocusPolicy(QWidget::StrongFocus);
    QWidget *w = new QWidget(this);
    w->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    m_layout = new QVBoxLayout(w);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->setAlignment(Qt::AlignCenter);
    setWidget(w);

    recreatePageWidget();
    
    setWidgetResizable(true);
    
    setBackground(props.background());
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

SimplePageView::~SimplePageView()
{
}

void SimplePageView::recreatePageWidget()
{
    delete imgLabel;
    imgLabel = NULL;

    // size of default empty page widget (until first image is loaded)
    int w = viewport()->width() - 10;
    int h = viewport()->height() - 10;

    if (numOfPages())
    {
        imgLabel = new PageWidget(this, w, h, 0, props.twoPagesMode());
        m_layout->addWidget(imgLabel); 
    }
}

void SimplePageView::setNumOfPages(int n)
{
    PageViewBase::setNumOfPages(n);
    recreatePageWidget();
}

void SimplePageView::propsChanged()
{
    qDebug() << "SimplePageView::propsChanged()";
    if (imgLabel)
    {
        if ((props.twoPagesMode() && !imgLabel->hasTwoPages()) || (imgLabel->hasTwoPages() && !props.twoPagesMode()))
        {
            recreatePageWidget();
        }
        imgLabel->redrawImages();
        update();
        gotoPage(m_currentPage);
    }    
}

void SimplePageView::scrollContentsBy(int dx, int dy)
{
    PageViewBase::scrollContentsBy(dx, dy);
}

void SimplePageView::setImage(const Page &img1)
{
    Q_ASSERT(numOfPages() > 0);
    delRequest(img1.getNumber(), false, false);
    if (img1.getNumber() == m_currentPage)
    {
        imgLabel->setImage(img1);
        horizontalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
        verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
    }
}

void SimplePageView::setImage(const Page &img1, const Page &img2)
{
    Q_ASSERT(numOfPages() > 0);
    delRequest(img1.getNumber(), true, false);
    if (img1.getNumber() == m_currentPage)
    {
        imgLabel->setImage(img1, img2);
        horizontalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
        verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);        
    }
}

void SimplePageView::gotoPage(int n)
{
    if (n>= 0 && n < numOfPages())
    {
        if (n != m_currentPage)
        {
            //delRequest(m_currentPage);
        }
        m_currentPage = n = roundPageNumber(n);

        addRequest(m_currentPage, props.twoPagesMode() && n+1 < numOfPages());
        emit currentPageChanged(n);

        ComicBookSettings &cfg(ComicBookSettings::instance());
        if (cfg.preloadPages())
        {   
            n = nextPage(n);
            qDebug() << "preloading" << n;            
            addRequest(n, props.twoPagesMode() && n < numOfPages()); // request two pages if not last page
        }
    }
}

void SimplePageView::resizeEvent(QResizeEvent *e)
{
    if (imgLabel)
    {
        imgLabel->redrawImages();
    }
    PageViewBase::resizeEvent(e);
}

void SimplePageView::wheelEvent(QWheelEvent *e)
{
    
    if (e->delta() > 0) //scrolling up
    {
        if (imgLabel->height() <= height() || (onTop() && ++wheelupcnt > EXTRA_WHEEL_SPIN))
        {
            e->accept();
            wheelupcnt = 0;
            gotoPage(previousPage(m_currentPage));
        }
        else
        {
            QScrollArea::wheelEvent(e);
            wheeldowncnt = 0; //reset opposite direction counter
        }
    }
    else //scrolling down
    {
        if (imgLabel->height() <= height() || (onBottom() && ++wheeldowncnt > EXTRA_WHEEL_SPIN))
        {
            e->accept();
            wheeldowncnt = 0;
            gotoPage(nextPage(m_currentPage));
        }
        else
        {
            QScrollArea::wheelEvent(e);
            wheelupcnt = 0; //reset opposite direction counter
        }
    }
}

void SimplePageView::scrollToTop()
{
	verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
}

void SimplePageView::scrollToBottom()
{
	verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}

void SimplePageView::clear()
{
    delRequests();
    setNumOfPages(0);
}
			
int SimplePageView::visiblePages() const
{
    return imgLabel ? imgLabel->numOfPages() : 0;
}

int SimplePageView::viewWidth() const
{
    return width(); //TODO?
//    return (imgLabel->numOfPages()) ? imgLabel->width() : 0;
}

int SimplePageView::currentPage() const
{
    return imgLabel ? imgLabel->pageNumber() : -1;
}

