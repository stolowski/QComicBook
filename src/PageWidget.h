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

#include <QWidget>
#include <QPixmap>

namespace QComicBook
{
    class Page;
    class PageViewBase;

    class PageWidget: public QWidget
    {
    Q_OBJECT
            
    public slots:
        void propsChanged();

    public:
        PageWidget(PageViewBase *parent, int pageNum, bool twoPages=false);
        virtual ~PageWidget();

        void setImage(const Page &img1);
        void setImage(const Page &img1, const Page &img2);
        void dispose();
        bool isDisposed() const;
        const QPixmap* pixmap() const;
        bool isInView(int vy1, int vy2) const;
        virtual void paintEvent(QPaintEvent *event);
        virtual void resizeEvent(QResizeEvent *event);
//        virtual QSize sizeHint() const;
        void redrawImages();
        void setEstimatedSize(int w, int h);
        bool estimatedSize() const;
        int pageNumber() const;
        bool hasTwoPages() const;

    protected:
        int numOfPages() const;
        void deletePages();
        void drawPageNumber(int page, QPainter &p, int x, int y);

    private:
        PageViewBase *view;
        int m_pageNum; //number of physical page
        Page *m_image[2];
        int xoff, yoff;
        QPixmap *m_pixmap;
        QSize pageSize; //size of 1 or 2 pages without scaling
        bool estimated;
        bool m_twoPages; //whether this widget holds one or two pages; this is independent from current two pages mode setting
    };
}

#endif
