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

#ifndef __LENS_H
#define __LENS_H

#include <QGraphicsItem>
#include <QSharedPointer>

class QPixmap;
class QTime;

namespace QComicBook
{
    class Lens: public QGraphicsItem
    {
    public:
        Lens(const QSize &size = QSize(300, 200), double ratio=2.0f, int delay=25);
        ~Lens();

	void setZoom(double ratio);
        QRectF boundingRect() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *widget = 0);

    protected:
        QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    private:
        QSharedPointer<QPixmap> m_pixmap;
        QTime *m_time;
        QSize m_size;
	double m_ratio;
        int m_delay;
    };
}

#endif
