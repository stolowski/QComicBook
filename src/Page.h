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

/*! \file Page.h */

#ifndef __PAGE_H
#define __PAGE_H

#include <QImage>

namespace QComicBook
{
    class Page
    {
    public:
        Page(unsigned int number, const QImage &image);
        ~Page();

        QImage getImage() const;
        unsigned int getNumber() const;
        int width() const;
        int height() const;

        operator const QImage&() const { return m_image; }
        operator int() const { return m_number; }

    private:
        const unsigned int m_number;
        const QImage m_image;
    };
}

#endif
