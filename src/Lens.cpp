#include "Lens.h"
#include "PageWidget.h"
#include "PageViewBase.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QPixmap>
#include <QTime>
#include <QtAlgorithms>
#include <QScrollBar>
#include <QDebug>

using namespace QComicBook;

bool cmpItemsY(const QGraphicsItem *i1, const QGraphicsItem *i2)
{
    return i1->y() < i2->y();
}

Lens::Lens(PageViewBase *view): QGraphicsItem()
                              , m_view(view)
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
    if (change == ItemPositionChange && scene() && m_time->elapsed() > 25)
    {
        QPointF newPos = value.toPointF(); //lens global position (scroll area coordinates)

        if (!m_pixmap)
        {
            m_pixmap = new QPixmap(300, 200);
        }
        hide(); // hide lens so that they are not rendered by view

        QPainter painter(m_pixmap);
        m_view->render(&painter, QRectF(0, 0, 300, 200), QRect(newPos.x()-75, newPos.y()-50-m_view->verticalScrollBar()->value(), 150, 100));
        painter.end();

        show();        
        m_time->restart();
    }
    return QGraphicsItem::itemChange(change, value);
}
