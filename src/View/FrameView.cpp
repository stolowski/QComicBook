/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "FrameView.h"
#include "ComicBookSettings.h"
#include "Page.h"
#include "FrameWidget.h"
#include <ComicFrameList.h>
#include <QPixmap>
#include <QVBoxLayout>
#include <QPainter>
#include <QScrollBar>
#include "ComicBookDebug.h"

using namespace QComicBook;

FrameView::FrameView(QWidget *parent, int physicalPages, const ViewProperties& props)
    : PageViewBase(parent, physicalPages, props)
	, m_currentPage(0) //??
	, m_currentFrame(0)
	, m_frame(0)
{
    /*  QWidget *w = new QWidget(this);
    w->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    m_layout = new QVBoxLayout(w);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->setAlignment(Qt::AlignCenter);
    setWidget(w);*/

	m_frame = new FrameWidget(this, viewport()->width() - 10, viewport()->height() - 10);
	scene->addItem(m_frame);

    ///setWidgetResizable(true);
    
    setBackground(props.background());
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

FrameView::~FrameView()
{

}
			
void FrameView::setImage(const Page &img1)
{
    _DEBUG << img1.getNumber();
    if (img1.getNumber() == m_currentPage)
    {
        m_page = img1;
    }
}

void FrameView::setImage(const Page &img1, const Page &img2)
{   
    _DEBUG << img1.getNumber();
    if (img1.getNumber() == m_currentPage)
    {
        m_page = img1;
    }
}

void FrameView::setFrames(const ComicFrameList &frames)
{
    _DEBUG << frames.pageNumber() << frames.count();
    if (frames.pageNumber() == m_currentPage)
    {
        m_frames = frames;
        m_frames.sort(props.mangaMode());
        m_currentFrame = 0;
        gotoFrame(m_currentFrame);
    }
}

void FrameView::nextFrame()
{
	if (m_currentFrame < m_frames.count() - 1)
	{
		++m_currentFrame;
		gotoFrame(m_currentFrame);
	}
	else
	{
		gotoPage(m_currentPage + 1);
	}
}

void FrameView::prevFrame()
{
	if (m_currentFrame > 0)
	{
		--m_currentFrame;
		gotoFrame(m_currentFrame);
	}
	else
	{
		gotoPage(m_currentPage - 1);
	}
}

void FrameView::gotoFrame(int n)
{
	if (n < m_frames.count())
	{
		m_currentFrame = n;
		const ComicFrame f(m_frames[n]);
		m_frame->setFrame(m_page, f);
                center(m_frame);
                setSceneRect(scene->itemsBoundingRect());
	}
}

void FrameView::clear()
{
	m_frame->clear();
}

void FrameView::gotoPage(int n)
{
	if (n>= 0 && n < numOfPages())
    {
        m_currentPage = n = roundPageNumber(n);

        addRequest(m_currentPage, false);
        emit currentPageChanged(n);

        ComicBookSettings &cfg(ComicBookSettings::instance());
        if (cfg.preloadPages() && n < numOfPages())
        {   
            ++n;
            _DEBUG << "preloading" << n;            
            addRequest(n, false);
        }
    }
}

void FrameView::scrollToTop()
{

	verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
}

void FrameView::scrollToBottom()
{
	verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}
            
void FrameView::propsChanged()
{
	m_frames.sort(props.mangaMode());
	gotoFrame(m_currentFrame);
}

void FrameView::jobCompleted(const ImageJobResult &result)
{
    _DEBUG;
}

int FrameView::visiblePages() const
{
	return 1;
}

int FrameView::viewWidth() const
{

    return width(); //TODO?
}

int FrameView::currentPage() const
{
	return m_currentPage;
}

void FrameView::resizeEvent(QResizeEvent *e)
{
    if (m_frame)
    {
        m_frame->recalcScaledSize();
		m_frame->redrawScaledImage();
    }
    PageViewBase::resizeEvent(e);
}

