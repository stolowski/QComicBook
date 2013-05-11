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

#include "ComicFrameImage.h"
#include "View/FrameView.h"
#include <ComicFrame.h>
#include "Page.h"
#include <QPainter>
#include <QImage>
#include "ComicBookDebug.h"
#include "Job/FrameRedrawJob.h"
#include "Job/ImageJobResult.h"
#include "Job/JobType.h"

using namespace QComicBook;

ComicFrameImage::ComicFrameImage(FrameView *parent, int w, int h)
    : ComicImage(parent)
    , m_image(0), m_framekey(-1)
{
}

ComicFrameImage::~ComicFrameImage()
{
    delete m_image;
}

void ComicFrameImage::setFrame(const Page &p, const ComicFrame &f)
{
    _DEBUG;
    m_image = new QImage(p.getImage());
    m_frame = QRect(f.xPos(), f.yPos(), f.width(), f.height());
    m_framekey = ((f.xPos() & 0xffff) << 16) | (f.yPos() & 0xffff);
    
    setSourceSize(f.width(), f.height());
}

ImageTransformJob* ComicFrameImage::createRedrawJob()
{
    _DEBUG;
    FrameRedrawJob *j = NULL;
    if (m_image)
    {
        j = new FrameRedrawJob();
        j->setKey(JobKey(FRAME_REDRAW, m_framekey));
        j->setImage(*m_image, m_frame);
    }
    return j;
}

bool ComicFrameImage::jobCompleted(const ImageJobResult &result)
{
    _DEBUG << result.key;
    if (m_image && result.key.getKey() == m_framekey)
    {
        _DEBUG << "matching job" << result.key;
        redraw(result.image);
        return true;
    }
    return false;
}

void ComicFrameImage::clear()
{
	delete m_image;
	m_image = 0;
	dispose();
}
			
void ComicFrameImage::propsChanged()
{
    _DEBUG;
}
