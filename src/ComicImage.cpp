/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2012 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "ComicImage.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "ComicBookSettings.h"
#include "View/PageViewBase.h"
#include "ImageTransformJob.h"
#include "ImageJobResult.h"
#include "ImageTransformThread.h"
#include "ComicBookDebug.h"

using namespace QComicBook;

ComicImage::ComicImage(PageViewBase *parent)
    : QGraphicsItem()
    , m_view(parent)
    , m_sourceSize(0, 0)
    , m_scaledSize(0, 0)
    , m_pixmap(0)
{
}

ComicImage::~ComicImage()
{
    delete m_pixmap;
}

void ComicImage::dispose()
{
    delete m_pixmap;
    m_pixmap = 0;
//	m_scaledSize = QSize(0, 0); //?
}

bool ComicImage::isDisposed() const
{
    return m_pixmap == NULL;
}

QRectF ComicImage::boundingRect() const
{
    return QRectF(0.0f, 0.0f, m_scaledSize.width(), m_scaledSize.height());
}

bool ComicImage::isInView(int vy1, int vy2) const
{
    const int y1 = pos().y();
    return std::min(y1 + m_scaledSize.height(), vy2) > std::max(y1, vy1);
}

void ComicImage::setSourceSize(int w, int h)
{
    m_sourceSize = QSize(w, h);
    recalcScaledSize();
}

QSize ComicImage::getSourceSize() const
{
    return m_sourceSize;
}

QSize ComicImage::getScaledSize() const
{
    return m_scaledSize;
}

int ComicImage::width() const
{
    return m_scaledSize.width();
}

int ComicImage::height() const
{
    return m_scaledSize.height();
}

void ComicImage::recalcScaledSize()
{
    _DEBUG;

    const int viewW(m_view->viewport()->width());
    const int viewH(m_view->viewport()->height());

    int totalWidth, totalHeight;
    ViewProperties &props(m_view->properties());

    if (props.angle() == 0 || props.angle() == 2)
    {
        totalWidth = m_sourceSize.width();
        totalHeight = m_sourceSize.height();
    }
    else
    {
        totalWidth = m_sourceSize.height();
        totalHeight = m_sourceSize.width();
    }
    Size size = props.size();
    
    const double hRatio = static_cast<double>(viewH) / totalHeight;
    const double wRatio = static_cast<double>(viewW) / totalWidth;

    int pixmapWidth, pixmapHeight; //resulting image size (with scaling and rotation applied)

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
        pixmapWidth = viewW;
        pixmapHeight = static_cast<int>(static_cast<double>(totalHeight) * wRatio);
    }
    else if (size == FitHeight)
    {
        pixmapWidth = static_cast<int>(static_cast<double>(totalWidth) * hRatio);
        pixmapHeight = viewH;
    }
    else if (size == WholePage)
    {
        const double ratio = std::min(wRatio, hRatio);
        pixmapWidth = static_cast<int>(static_cast<double>(ratio) * totalWidth);
        pixmapHeight = static_cast<int>(static_cast<double>(ratio) * totalHeight);
    }
    
    xoff = (viewW - pixmapWidth) / 2;
    yoff = props.continuousScrolling() ? 0 : (viewH - pixmapHeight) / 2;
    
    prepareGeometryChange();

    m_scaledSize = QSize(pixmapWidth, pixmapHeight);
    
    if (xoff < 0)
        xoff = 0;
    if (yoff < 0)
        yoff = 0;
	
    rmtx.reset();   
    if (props.angle() > 0)
    {
        if (props.angle() == 1)
            rmtx.translate(m_scaledSize.width(), 0);
        else if (props.angle() == 3)
            rmtx.translate(0, m_scaledSize.height());
        else
            rmtx.translate(m_scaledSize.width(), m_scaledSize.height());
        rmtx.rotate(static_cast<double>(props.angle()) * 90.0f);
    }
    
    rmtx.scale(static_cast<double>(pixmapWidth)/totalWidth, static_cast<double>(pixmapHeight)/totalHeight);
	
    //setContentsMargins(xoff, yoff, 0, 0);
    //setFixedSize(m_scaledSize.width() + 2*xoff, m_scaledSize.height() + 2*yoff);  

    requestRedraw(m_scaledSize, rmtx);

    //updateGeometry();
    update();
}

void ComicImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *widget)
{
    if (m_pixmap)
    {
//      painter->drawPixmap(opt->rect().x(), opt->rect().y(), *m_pixmap, opt->rect().x()-xoff, opt->rect().y()-yoff, opt->rect().width(), opt->rect().height());
        painter->drawPixmap(opt->exposedRect, *m_pixmap, opt->exposedRect);
    }
}

void ComicImage::requestRedraw(const QSize& requestedSize, const QMatrix &rotationMatrix)
{
    ViewProperties &props(m_view->properties());

    ImageTransformJob *j = createRedrawJob();
    if (j)
    {
        j->setSize(requestedSize.width(), requestedSize.height());
        j->setMatrix(rotationMatrix);
        ImageTransformThread::get()->addJob(j);
    }
}

void ComicImage::redraw(const QImage &img)
{
    _DEBUG;
    if (img.size() == m_scaledSize) // sanity check; should match size requested in requestRedraw(..)
    {
        if (m_pixmap != NULL && m_pixmap->size() == img.size()) // reuse existing pixmap if of right size
        {
            QPainter p(m_pixmap);
            p.drawImage(0, 0, img);
            p.end();
        }
        else
        {
            delete m_pixmap;
            m_pixmap = new QPixmap(QPixmap::fromImage(img));
        }
        update();
    }
    else
    {
        qWarning() << "redraw() called with wrong size";
    }
}

const QPixmap* ComicImage::pixmap() const
{
    return m_pixmap;
}

PageViewBase* ComicImage::view() const
{
    return m_view;
}
