/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2009 Pawel Stolowski <pawel.stolowski@wp.pl>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "Page.h"

using namespace QComicBook;

Page::Page(unsigned int number, const QImage &image)
    : m_number(number),
      m_image(image)
{
}

Page::~Page()
{
}

QImage Page::getImage() const
{
    return m_image;
}

unsigned int Page::getNumber() const
{
    return m_number;
}

int Page::width() const
{
    return m_image.width();
}

int Page::height() const
{
    return m_image.height();
}
