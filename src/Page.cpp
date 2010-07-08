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

#include "Page.h"

using namespace QComicBook;

Page::Page()
    : m_number(-1)
{
}

Page::Page(const Page &p)
{
    m_number = p.m_number;
    m_image = p.m_image;
}

Page::Page(int number, const QImage &image)
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

int Page::getNumber() const
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
