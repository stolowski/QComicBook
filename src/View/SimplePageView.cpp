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
#include "../ComicBookDebug.h"

using namespace QComicBook;

const int SimplePageView::EXTRA_WHEEL_SPIN = 3;

SimplePageView::SimplePageView(QWidget *parent, int physicalPages, const ViewProperties& props)
    : PageViewBase(parent, physicalPages, props)
    , wheelupcnt(0), wheeldowncnt(0)
    , m_currentPage(0) //??
    , imgLabel(NULL)
{
    _DEBUG;

    recreatePageWidget();
    
    setBackground(props.background());
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

SimplePageView::~SimplePageView()
{
    _DEBUG;
}

void SimplePageView::recreatePageWidget()
{
    _DEBUG;

    delete imgLabel;
    imgLabel = NULL;

    // size of default empty page widget (until first image is loaded)
    int w = viewport()->width() - 10;
    int h = viewport()->height() - 10;

    if (numOfPages())
    {
        imgLabel = new PageWidget(this, w, h, 0, props.twoPagesMode());
        scene->addItem(imgLabel);
    }
}

void SimplePageView::setNumOfPages(int n)
{
    PageViewBase::setNumOfPages(n);
    recreatePageWidget();
}

void SimplePageView::propsChanged()
{
    _DEBUG;
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
    setSceneRect(scene->itemsBoundingRect());
}

void SimplePageView::jobCompleted(const ImageJobResult &result)
{
    _DEBUG;
    if (imgLabel)
    {
        imgLabel->jobCompleted(result);
        setSceneRect(scene->itemsBoundingRect());
    }
}

void SimplePageView::scrollContentsBy(int dx, int dy)
{
    PageViewBase::scrollContentsBy(dx, dy);
}

void SimplePageView::setImage(const Page &img1)
{
    Q_ASSERT(numOfPages() > 0);
    _DEBUG << img1.getNumber();
    delRequest(img1.getNumber(), false, false);
    if (img1.getNumber() == m_currentPage)
    {
        imgLabel->setImage(img1);
        center(imgLabel);
        setSceneRect(scene->itemsBoundingRect());
        horizontalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
        verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
        emit pageReady(img1);
    }
}

void SimplePageView::setImage(const Page &img1, const Page &img2)
{
    Q_ASSERT(numOfPages() > 0);
    _DEBUG << img1.getNumber() << "," << img2.getNumber();
    delRequest(img1.getNumber(), true, false);
    if (img1.getNumber() == m_currentPage)
    {
        imgLabel->setImage(img1, img2);
        center(imgLabel);
        setSceneRect(scene->itemsBoundingRect());
        horizontalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
        verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
        emit pageReady(img1, img2);
    }
}

void SimplePageView::gotoPage(int n)
{
    _DEBUG << n;
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
            _DEBUG << "preloading" << n;            
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
    if (imgLabel)
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
                QGraphicsView::wheelEvent(e);
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
                QGraphicsView::wheelEvent(e);
                wheelupcnt = 0; //reset opposite direction counter
            }
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

