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

#ifndef __PAGEREDRAWJOB_H
#define __PAGEREDRAWJOB_H

#include "ImageTransformJob.h"

class QImage;

namespace QComicBook
{
    class Page;

    class PageRedrawJob: public ImageTransformJob
    {
    public:
        PageRedrawJob();
        ~PageRedrawJob();

        void setImage(const Page &p1);
        void setImage(const Page &p1, const Page &p2);
        void setSourceSize(const QSize &size);

        void execute();
        QImage getResult() const;

    protected:
        void drawPageNumber(int page, QPainter &p, int x, int y);

    private:
        QImage *m_image[2];
        int m_numbers[2];  //!< page numbers
        QImage *m_result; //!< resulting image
        QSize m_sourceSize;
    };
}

#endif
