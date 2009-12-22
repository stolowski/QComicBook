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

#include "Lens.h"
#include "PageWidget.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QPixmap>
#include <QTime>
#include <QtAlgorithms>
#include <QScrollBar>
#include <QGraphicsScene>
#include <QDebug>

using namespace QComicBook;

bool cmpItemsY(const QGraphicsItem *i1, const QGraphicsItem *i2)
{
    return i1->y() < i2->y();
}

Lens::Lens(): QGraphicsItem()
            , m_pixmap(0)
{
    m_time = new QTime();
    m_time->start();
    setZValue(1000.0f);
}

Lens::~Lens()
{
    delete m_pixmap;
    delete m_time;
}

void Lens::paint(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *widget)
{
    if (m_pixmap)
    {
        painter->drawPixmap(opt->exposedRect, *m_pixmap, QRect(opt->exposedRect.x()+150, opt->exposedRect.y()+100, opt->exposedRect.width(), opt->exposedRect.height()));
        painter->setPen(QPen(Qt::red, 1.0f));
        painter->drawRect(-150, -100, 299, 199);
    }
}

QRectF Lens::boundingRect() const
{
    return QRectF(-150, -100, 300, 200);
}

QVariant Lens::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene() && m_time->elapsed() > 25)
    {
        QPointF newPos = value.toPointF(); //lens global position (scroll area coordinates)

        if (!m_pixmap)
        {
            m_pixmap = new QPixmap(300, 200);
            m_pixmap->fill(); //TODO bg color?
        }
        hide(); // hide lens so that they are not rendered by view

        Q_ASSERT(scene() != 0);
        QPainter painter(m_pixmap);
        scene()->render(&painter, QRectF(0, 0, 300, 200), QRect(newPos.x()-75, newPos.y()-50, 150, 100));
        painter.end();

        show();        
        m_time->restart();
    }
    return QGraphicsItem::itemChange(change, value);
}
