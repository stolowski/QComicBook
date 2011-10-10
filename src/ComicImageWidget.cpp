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

#include "ComicImageWidget.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "ComicBookSettings.h"
#include "View/PageViewBase.h"
#include <QDebug>

using namespace QComicBook;

ComicImageWidget::ComicImageWidget(PageViewBase *parent)
	: QGraphicsItem()
	  , m_view(parent)
	  , imageSize(0, 0)
	  , scaledSize(0, 0)
	  , m_pixmap(0)
{
}

ComicImageWidget::~ComicImageWidget()
{
	delete m_pixmap;
}

void ComicImageWidget::dispose()
{
    delete m_pixmap;
    m_pixmap = 0;
	scaledSize = QSize(0, 0); //?
}

bool ComicImageWidget::isDisposed() const
{
    return m_pixmap == NULL;
}

QRectF ComicImageWidget::boundingRect() const
{
    return QRectF(0.0f, 0.0f, scaledSize.width(), scaledSize.height());
}

bool ComicImageWidget::isInView(int vy1, int vy2) const
{
    const int y1 = pos().y();
    return std::min(y1 + scaledSize.height(), vy2) > std::max(y1, vy1);
}

void ComicImageWidget::setSourceSize(int w, int h)
{
	imageSize = QSize(w, h);
	recalcScaledSize();
	//redrawScaledImage();
}

QSize ComicImageWidget::getSourceSize() const
{
	return imageSize;
}

QSize ComicImageWidget::getScaledSize() const
{
	return scaledSize;
}

int ComicImageWidget::width() const
{
	return scaledSize.width();
}

int ComicImageWidget::height() const
{
	return scaledSize.height();
}

void ComicImageWidget::recalcScaledSize()
{
    qDebug() << "ComicImageWidget::recalcScaledSize";

    const int viewW(m_view->viewport()->width());
    const int viewH(m_view->viewport()->height());

    int totalWidth, totalHeight;
    ViewProperties &props(m_view->properties());

	if (props.angle() == 0 || props.angle() == 2)
	{
		totalWidth = imageSize.width();
		totalHeight = imageSize.height();
	}
	else
	{
		totalWidth = imageSize.height();
		totalHeight = imageSize.width();
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

    scaledSize = QSize(pixmapWidth, pixmapHeight);
    
    if (xoff < 0)
        xoff = 0;
    if (yoff < 0)
		yoff = 0;
	
	rmtx.reset();   
	if (props.angle() > 0)
	{
		if (props.angle() == 1)
			rmtx.translate(scaledSize.width(), 0);
		else if (props.angle() == 3)
			rmtx.translate(0, scaledSize.height());
		else
			rmtx.translate(scaledSize.width(), scaledSize.height());
		rmtx.rotate(static_cast<double>(props.angle()) * 90.0f);
	}

	rmtx.scale(static_cast<double>(pixmapWidth)/totalWidth, static_cast<double>(pixmapHeight)/totalHeight);
	
	//setContentsMargins(xoff, yoff, 0, 0);
	//setFixedSize(scaledSize.width() + 2*xoff, scaledSize.height() + 2*yoff);  

	//updateGeometry();
	update();
}

void ComicImageWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *widget)
{
    if (m_pixmap)
    {
//        painter->drawPixmap(opt->rect().x(), opt->rect().y(), *m_pixmap, opt->rect().x()-xoff, opt->rect().y()-yoff, opt->rect().width(), opt->rect().height());
        painter->drawPixmap(opt->exposedRect, *m_pixmap, opt->exposedRect);
    }
}

void ComicImageWidget::redrawScaledImage()
{
	qDebug() << "redrawScaledImage";

    ViewProperties &props(m_view->properties());

	if (scaledSize.width() > 0)
	{
		if (m_pixmap == NULL || m_pixmap->width() != scaledSize.width() || m_pixmap->height() != scaledSize.height())
		{
			delete m_pixmap;
			m_pixmap = new QPixmap(scaledSize.width(), scaledSize.height());
		}
		QPainter p(m_pixmap);
		p.setRenderHint(QPainter::SmoothPixmapTransform, ComicBookSettings::instance().smoothScaling());

		p.setWorldMatrix(rmtx);
		p.setWorldMatrixEnabled(true);

		redraw(p);

		p.end();

		/*  setContentsMargins(xoff, yoff, 0, 0);
		setFixedSize(scaledSize.width() + 2*xoff, scaledSize.height() + 2*yoff);  

		updateGeometry();*/
		update();
	}
}

const QPixmap* ComicImageWidget::pixmap() const
{
    return m_pixmap;
}

PageViewBase* ComicImageWidget::view() const
{
	return m_view;
}
