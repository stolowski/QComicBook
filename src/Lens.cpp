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

#include "Lens.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QPixmap>
#include <QTime>
#include <QGraphicsScene>
#include "ComicBookDebug.h"

using namespace QComicBook;

Lens::Lens(const QSize &size, const QColor &background, double ratio, int delay): QGraphicsItem()
                                        , m_pixmap(0)
                                        , m_time(0)
                                        , m_delay(delay)
                                        , m_size(size)
                                        , m_background(background)
					, m_ratio(ratio)
{
    setZValue(1000.0f);
    setFlags(ItemSendsGeometryChanges);
}

Lens::~Lens()
{
    m_pixmap.clear();
    delete m_time;
}

void Lens::setBackground(const QColor &bg)
{
    m_background = bg;
    update();
}

void Lens::setZoom(double ratio)
{
	m_ratio = ratio;
	m_pixmap.clear();
	// TODO force redraw
}

void Lens::paint(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *widget)
{
    _DEBUG;
    if (m_pixmap)
    {
        painter->drawPixmap(opt->exposedRect, *m_pixmap, QRectF(0, 0, m_pixmap->width(), m_pixmap->height()));
        painter->setPen(QPen(Qt::black, 1.0f));
        painter->drawRect(-m_size.width()/2, -m_size.height()/2, m_size.width(), m_size.height());
    }
}

QRectF Lens::boundingRect() const
{
    return QRectF(-m_size.width()/2, -m_size.height()/2, m_size.width(), m_size.height());
}

QVariant Lens::itemChange(GraphicsItemChange change, const QVariant &value)
{
    _DEBUG << change;

    if (scene() && (change == ItemVisibleChange || (change == ItemPositionChange && (m_time == NULL || m_time->elapsed() > m_delay))))
    {
        QPointF newPos = (change == ItemVisibleChange) ? pos() : value.toPointF(); //lens global position (scroll area coordinates)
 
	QList<QGraphicsItem*> items = scene()->collidingItems(this, Qt::IntersectsItemBoundingRect);
	if (items.size() == 0)
	{
             m_pixmap.clear();
	}
	else
	{
		if (!m_pixmap)
		{
		    m_pixmap = QSharedPointer<QPixmap>(new QPixmap(m_size.width()/m_ratio, m_size.height()/m_ratio));
                }

                //
                // clear lens; this is a bit inefficient, but easy. 
                // optimal way would be to only clear strips that 
                // won't be painted.
                m_pixmap->fill(m_background);

		QPainter painter(m_pixmap.data());
		
		QStyleOptionGraphicsItem so;
			
		QRectF tbr(boundingRect());
		//
		// translate bounding rect coordinates to global coords
		tbr.translate(newPos.x(), newPos.y());
		tbr.translate((tbr.width()-(tbr.width()/m_ratio))/2, (tbr.height() - (tbr.height()/m_ratio))/2);
		tbr.setWidth(tbr.width()/m_ratio);
		tbr.setHeight(tbr.height()/m_ratio);

		foreach (QGraphicsItem *it, items)
		{
			//
			// translate item rect to global coords
			QRectF itbr(it->boundingRect());
			itbr.translate(it->x(), it->y());

			//
			// get rect intersection
			QRectF ins(itbr.intersected(tbr));

			_DEBUG << "lens intersect" << itbr << "&" << tbr << "=" << ins;

			const int xoff(ins.x() - tbr.x());
			const int yoff(ins.y() - tbr.y());
			_DEBUG << "off" << xoff << yoff;

			ins.translate(-it->x(), -it->y());
			so.exposedRect = ins;
			_DEBUG << "exposed" << so.exposedRect;
		
			painter.resetTransform();
			painter.translate(-ins.x() + xoff, -ins.y() + yoff);

			it->paint(&painter, &so);
		}	
		painter.end();

                if (m_time)
                {
                    m_time->restart();
                }
                else
                {
                    m_time = new QTime();
                    m_time->start();
                }
        }
    }
    return QGraphicsItem::itemChange(change, value);
}
