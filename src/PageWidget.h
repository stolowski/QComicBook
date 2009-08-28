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
    class ComicImageView;

    class PageWidget: public QWidget
    {
    Q_OBJECT
            
    public slots:
        void propsChanged();

    public:
        PageWidget(ComicImageView *parent);
        virtual ~PageWidget();

        void setImage(const Page &img1);
        void setImage(const Page &img1, const Page &img2);
        void dispose();
        const QPixmap* pixmap() const;
        int numOfPages() const;

        virtual void paintEvent(QPaintEvent *event);
        virtual void resizeEvent(QResizeEvent *event);
//        virtual QSize sizeHint() const;
        void redrawImages();
        void setEstimatedSize(int w, int h);
        bool estimatedSize() const;

    protected:
        void deletePages();
        void drawPageNumber(int page, QPainter &p, int x, int y);

    private:
        ComicImageView *view;
        Page *m_image[2];
        int xoff, yoff;
        QPixmap *m_pixmap;
        QSize pageSize; //size of 1 or 2 pages without scaling
        bool estimated;
    };
}

#endif
