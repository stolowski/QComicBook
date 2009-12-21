#include "Lens.h"
#include "PageWidget.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QPixmap>
#include <QTime>
#include <QtAlgorithms>
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
}

void Lens::paint(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *widget)
{
    if (m_pixmap)
    {
        painter->drawPixmap(opt->exposedRect, *m_pixmap, QRect(opt->exposedRect.x()+150, opt->exposedRect.y()+100, opt->exposedRect.width(), opt->exposedRect.height()));
        painter->setPen(QPen(Qt::red, 1.0f));
        //painter->drawRect(0, 0, 299, 199);
        painter->drawRect(-150, -100, 299, 199);
    }
}

QRectF Lens::boundingRect() const
{
//    return QRectF(0, 0, 300, 200);
    return QRectF(-150, -100, 300, 200);
}

QVariant Lens::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene() && m_time->elapsed() > 10)
    {
        QPointF newPos = value.toPointF();
        qDebug() << "lens move" << newPos;

        if (!m_pixmap)
        {
            m_pixmap = new QPixmap(300, 200);
        }

        QList<QGraphicsItem *> clist = collidingItems(Qt::IntersectsItemBoundingRect);
        if (clist.size())
        {
            qSort(clist.begin(), clist.end(), cmpItemsY);
            QPainter ptr(m_pixmap);
            ptr.scale(2.0f, 2.0f);
            foreach (QGraphicsItem *it, clist)
            {
                PageWidget *w = dynamic_cast<PageWidget*>(it);
                Q_ASSERT(w != 0);
                QStyleOptionGraphicsItem st;
                
                QPointF f = newPos - w->pos() - QPointF(75, 50);
                ptr.translate(-f);
                qDebug() << "pos" << f;
                st.exposedRect = QRectF(f.x(), f.y(), 300, 200);
                w->paint(&ptr, &st);
            }
            ptr.end();
            update();
            m_time->restart();
        }
    }
    return QGraphicsItem::itemChange(change, value);
}
