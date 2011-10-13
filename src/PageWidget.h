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

#ifndef __PAGEWIDGET_H
#define __PAGEWIDGET_H

#include "ComicImageWidget.h"

namespace QComicBook
{
    class Page;
    class PageViewBase;

    class PageWidget: public ComicImageWidget
    {
    public:
        PageWidget(PageViewBase *parent, int w, int h, int pageNum, bool twoPages=false);
        virtual ~PageWidget();

        virtual void redraw(QPainter &p);
        ImageTransformJob *redrawJob();

        void setImage(const Page &img1);
        void setImage(const Page &img1, const Page &img2);
        Page getPage(int n);

        virtual void dispose();
		virtual bool isDisposed() const;

        void redrawImages();
        void setEstimatedSize(int w, int h);
        bool isEstimated() const;
        int pageNumber() const;
        bool hasTwoPages() const;
        int numOfPages() const;
        QSize estimatedSize() const;
		
        virtual void propsChanged();
        bool jobCompleted(const ImageJobResult &result);

    protected:
        void deletePages();
        void drawPageNumber(int page, QPainter &p, int x, int y);

    private:
        int m_pageNum; //number of physical page
        Page *m_image[2];
        QSize pageSize; //size of 1 or 2 pages without scaling
        bool estimated;
        bool m_twoPages; //whether this widget holds one or two pages; this is independent from current two pages mode setting
    };
}

#endif
