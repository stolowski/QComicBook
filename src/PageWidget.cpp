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

#include "PageWidget.h"
#include "Page.h"
#include "ComicImageView.h"
#include <QPaintEvent>
#include <QPainter>
#include <QSizePolicy>

using namespace QComicBook;

PageWidget::PageWidget(ComicImageView *parent)
    : QWidget(parent)
    , view(parent)
    , m_pixmap(NULL)
    , pageSize(1, 1)
    , estimated(true)
{
    m_image[0] = m_image[1] = NULL;
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(0, 0);
}

PageWidget::~PageWidget()
{
    dispose();
}

void PageWidget::deletePages()
{
    for (int i=0; i<2; i++)
    {
        delete m_image[i];
        m_image[i] = NULL;
    }
}

void PageWidget::setImage(const Page &img1)
{
    deletePages();
    m_image[0] = new Page(img1);
    pageSize = QSize(img1.width(), img1.height());
    estimated = false;
    redrawImages();
}

void PageWidget::setImage(const Page &img1, const Page &img2)
{
    deletePages();
    m_image[0] = new Page(img1);
    m_image[1] = new Page(img2);
    pageSize = QSize(img1.width() + img2.width(), std::max(img1.height(), img2.height()));
    estimated = false;
    redrawImages();
}

const QPixmap* PageWidget::pixmap() const
{
    return m_pixmap;
}

void PageWidget::dispose()
{
    delete m_pixmap;
    m_pixmap = NULL;
    deletePages();
}

void PageWidget::setEstimatedSize(int w, int h)
{
    if (estimated) // update size only if we have estimated size, otherwise we know last real size
    {
        if (pageSize.width() != w || pageSize.height() != h)
        {
            pageSize = QSize(w, h);
            redrawImages();
        }
    }
}

bool PageWidget::estimatedSize() const
{
    return estimated;
}

void PageWidget::drawPageNumber(int page, QPainter &p, int x, int y)
{
    const QString pagestr(QString::number(page));
    const QFontMetrics mtr(p.fontMetrics());
    const int txtw(mtr.width(pagestr));
    p.setPen(Qt::black);
    p.fillRect(x - txtw - 5, y - 2 - mtr.height(), txtw + 5, mtr.height() + 4, Qt::white);
    p.drawText(x - txtw - 4, y - 4, pagestr);
}

void PageWidget::redrawImages()
{
    const int pages = numOfPages();

    int totalWidth, totalHeight;
    ViewProperties &props = view->properties();

    if (pages == 0) // images not set or disposed, use last known or estimated size
    {
        if (props.angle() == 0 || props.angle() == 2)
        {
            totalWidth = pageSize.width();
            totalHeight = pageSize.height();
        }
        else
        {
            totalWidth = pageSize.height();
            totalHeight = pageSize.width();
        }
    }
    else if (pages == 1)
    {
        if (props.angle() == 0 || props.angle() == 2)
        {
            totalWidth = m_image[0]->width();
            totalHeight = m_image[0]->height();
        }
        else
        {
            totalWidth = m_image[0]->height();
            totalHeight = m_image[0]->width();
        }
    }
    else // 2 pages
    {
        if (props.angle() == 0 || props.angle() == 2)
        {
            totalWidth = m_image[0]->width() + m_image[1]->width();
            totalHeight = std::max(m_image[0]->height(), m_image[1]->height());
        }
        else
        {
            totalWidth = std::max(m_image[0]->height(), m_image[1]->height());
            totalHeight = m_image[0]->width() + m_image[1]->width();
        }
    }
    
    Size size = props.size();

    const double hRatio = static_cast<double>(view->height()) / totalHeight;
    const double wRatio = static_cast<double>(view->width()) / totalWidth;

    int pixmapWidth, pixmapHeight; //resulting image size (1 or 2 pages with scaling and rotation applied)

    if (size == BestFit)
    {
        if (totalWidth > totalHeight)
            size = FitWidth;
        else
            size = FitHeight;
    }
    if (size == Original)
    {
        pixmapWidth = totalWidth;
        pixmapHeight = totalHeight;
    }
    else if (size == FitWidth)
    {
        pixmapWidth = view->width();
        pixmapHeight = static_cast<int>(static_cast<double>(totalHeight) * wRatio);
    }
    else if (size == FitHeight)
    {
        pixmapWidth = static_cast<int>(static_cast<double>(totalWidth) * hRatio);
        pixmapHeight = view->height();
    }
    else if (size == WholePage)
    {
        const double ratio = std::min(wRatio, hRatio);
        pixmapWidth = static_cast<int>(static_cast<double>(ratio) * totalWidth);
        pixmapHeight = static_cast<int>(static_cast<double>(ratio) * totalHeight);
    }
    
    xoff = (view->width() - pixmapWidth) /2 ;
    yoff = (view->height() - pixmapHeight) /2;
    
    if (xoff < 0)
        xoff = 0;
    if (yoff < 0)
        yoff = 0;

    if (pages > 0)
    {
        if (m_pixmap == NULL || m_pixmap->width() != pixmapWidth || m_pixmap->height() != pixmapHeight)
        {
            delete m_pixmap;
            m_pixmap = new QPixmap(pixmapWidth, pixmapHeight);
        }
        QPainter p(m_pixmap);
 
        QMatrix rmtx;
        rmtx.reset();   
        if (props.angle() > 0)
        {
            if (props.angle() == 1)
                rmtx.translate(pixmapWidth, 0);
            else if (props.angle() == 3)
                rmtx.translate(0, pixmapHeight);
            else
                rmtx.translate(pixmapWidth, pixmapHeight);
            rmtx.rotate(static_cast<double>(props.angle()) * 90.0f);
            p.setWorldMatrix(rmtx);
            p.setWorldMatrixEnabled(true);
        }

        rmtx.scale(static_cast<double>(pixmapWidth)/totalWidth, static_cast<double>(pixmapHeight)/totalHeight);
        p.setWorldMatrix(rmtx);
        p.setWorldMatrixEnabled(true);
    
        if (pages == 1)
        {
            p.drawImage(0, 0, m_image[0]->getImage(), 0, 0);
            if (props.pageNumbers())
            {
                p.setWorldMatrixEnabled(false);
                drawPageNumber(m_image[0]->getNumber(), p, m_pixmap->width(), m_pixmap->height());
            }
        }
        else if (pages == 2)
        {
            // clear areas not covered by page (if pages sizes differ)
            for (int i=0; i<2; i++)
            {
                if (m_image[i]->height() < std::max(m_image[0]->height(), m_image[1]->height()))
                {
                    p.fillRect(i*m_image[0]->width(), m_image[i]->height(), m_image[i]->width(), totalHeight - m_image[i]->height(), props.background());
                    break; //only one page may be smaller
                }
            }
        
            p.drawImage(0, 0, m_image[0]->getImage(), 0, 0);
            p.drawImage(m_image[0]->width(), 0, m_image[1]->getImage(), 0, 0);
            if (props.pageNumbers())
            {
                p.setWorldMatrixEnabled(false);
                drawPageNumber(std::max(m_image[0]->getNumber(), m_image[1]->getNumber()), p, m_pixmap->width(), m_pixmap->height());
            }
        }
    
        p.end();
    }

    setContentsMargins(xoff, yoff, 0, 0);
    setFixedSize(pixmapWidth + 2*xoff, pixmapHeight + 2*yoff);  
  
    updateGeometry();
    update();
}

int PageWidget::numOfPages() const
{
    int n;
    for (n=0; n<2; n++)
    {
        if (!m_image[n])
            break;
    }
    return n;
}

void PageWidget::paintEvent(QPaintEvent *event)
{
    if (m_pixmap)
    {
        QPainter p(this);
        p.drawPixmap(event->rect().x(), event->rect().y(), *m_pixmap, event->rect().x()-xoff, event->rect().y()-yoff, event->rect().width(), event->rect().height());
    }
    event->accept();
}

void PageWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void PageWidget::propsChanged()
{
    redrawImages();
} 
