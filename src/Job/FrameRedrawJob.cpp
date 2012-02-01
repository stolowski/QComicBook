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

#include "FrameRedrawJob.h"
#include <QPainter>
#include "../ComicBookDebug.h"
#include <QImage>

using namespace QComicBook;

FrameRedrawJob::FrameRedrawJob(): ImageTransformJob(), m_img(0), m_result(0)
{
}

FrameRedrawJob::~FrameRedrawJob()
{
    _DEBUG;
    delete m_img;
    delete m_result;
}

void FrameRedrawJob::setImage(const QImage &img, const QRect &frame)
{
    delete m_img;
    m_img = new QImage(img);
    m_rect = frame;
}

void FrameRedrawJob::execute()
{
    _DEBUG << key();

    delete m_result;
    m_result = new QImage(m_width, m_height, QImage::Format_ARGB32);
    m_result->fill(0); // this prevents artifcats/garbage in transparent images

    QPainter p(m_result);
    p.setRenderHint(QPainter::SmoothPixmapTransform, m_props.smoothScaling);

    p.setWorldMatrix(*m_matrix);
    p.setWorldMatrixEnabled(true);

    p.drawImage(0, 0, *m_img, m_rect.x(), m_rect.y(), m_rect.width(), m_rect.height());

    p.end();
}

QImage FrameRedrawJob::getResult() const
{
    if (m_result)
    {
        return *m_result;
    }
    return QImage();
}
