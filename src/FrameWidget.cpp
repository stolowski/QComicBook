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

#include "FrameWidget.h"
#include "View/FrameView.h"
#include <ComicFrame.h>
#include "Page.h"
#include <QPainter>
#include <QImage>

using namespace QComicBook;

FrameWidget::FrameWidget(FrameView *parent, int w, int h)
	: ComicImageWidget(parent)
	  , m_image(0)
{
}

FrameWidget::~FrameWidget()
{
	delete m_image;
}

void FrameWidget::setFrame(const Page &p, const ComicFrame &f)
{
	m_image = new QImage(p.getImage());
	m_frame = QRect(f.xPos(), f.yPos(), f.width(), f.height());

	setSourceSize(f.width(), f.height());
	redrawScaledImage();
}

void FrameWidget::redraw(QPainter &p)
{
	if (m_image)
	{
		p.drawImage(0, 0, *m_image, m_frame.x(), m_frame.y(), m_frame.width(), m_frame.height());
	}
}

void FrameWidget::clear()
{
	delete m_image;
	m_image = 0;
	dispose();
}
			
void FrameWidget::propsChanged()
{
}
