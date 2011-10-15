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

#include "ImageTransformJob.h"
#include "ComicBookDebug.h"
#include <QMatrix>

using namespace QComicBook;

ImageTransformJob::ImageTransformJob(): m_matrix(0)
{
}

ImageTransformJob::~ImageTransformJob()
{
    _DEBUG;
    delete m_matrix;
}

void ImageTransformJob::setKey(const JobKey &k)
{
    m_key = k;
}

const JobKey& ImageTransformJob::key() const
{
    return m_key;
}

void ImageTransformJob::setSize(int w, int h)
{
    m_width = w;
    m_height = h;
}

void ImageTransformJob::setMatrix(const QMatrix &m)
{
    delete m_matrix;
    m_matrix = new QMatrix(m);
}
