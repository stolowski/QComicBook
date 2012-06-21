/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2012 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "ContinuousPageViewDebug.h"
#include "View/ContinuousPageView.h"
#include "PageWidget.h"
#include <QTextStream>

namespace QComicBook
{

ContinuousPageViewDebug::ContinuousPageViewDebug(QWidget *parent):
    QWidget(parent),
    m_view(NULL)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    m_scene = new QGraphicsScene(this);
    view->setScene(m_scene);

    setZoomFactor(zoomFactor->value());

    connect(zoomFactor, SIGNAL(valueChanged(double)), this, SLOT(setZoomFactor(double)));
    connect(refreshButton, SIGNAL(clicked()), this, SLOT(refreshClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

ContinuousPageViewDebug::~ContinuousPageViewDebug()
{
}

void ContinuousPageViewDebug::refreshClicked()
{
    if (m_view == NULL) {
        return;
    }

    refresh();
}

void ContinuousPageViewDebug::setZoomFactor(double v)
{
    QMatrix m;
    m.scale(v, v);
    view->setMatrix(m);
}

void ContinuousPageViewDebug::setView(ContinuousPageView *view)
{
    m_view = view;
}

void ContinuousPageViewDebug::refresh()
{
    m_scene->clear();
    if (m_view) {
        dumpTextData();
        drawRangeLines();
        drawPageRectangles();
    } else {
        textData->append("No continuous view");
    }
}

void ContinuousPageViewDebug::dumpTextData()
{
    QString text;
    QTextStream s(&text);
    s << "Number of PageWidget objs: " << m_view->imgLabel.size() << "\n";
    s << "Page description format: " << "d - disposed, e - estimated, pageNumber/estimatedSize; sourceSize; scaledSize / numOfPages" << "\n";
    textData->append(text);
}

void ContinuousPageViewDebug::drawRangeLines()
{
    const QPen pen(Qt::red, Qt::DotLine);
    for (int i=0; i < m_view->imgLabel.size(); i++) {
        int y1 = m_view->m_ypos.startCoordinate(i);
        int y2 = m_view->m_ypos.endCoordinate(i);
        m_scene->addLine(QLineF(-1000.0f, y1, 0.0f, y1), pen);
        m_scene->addLine(QLineF(-1000.0f, y2, 0.0f, y2), pen);
    }
}
void ContinuousPageViewDebug::drawPageRectangles()
{
    QFont font;
    font.setPixelSize(200);

    for (int i=0; i < m_view->imgLabel.size(); i++) {
        PageWidget *page = m_view->imgLabel[i];
        m_scene->addRect(page->x(), page->y(), page->width(), page->height(), QColor(Qt::blue));
        m_scene->addRect(page->x(), page->y(), page->width(), page->height(), QColor(Qt::blue));

        QString status = QString::number(page->m_pageNum + 1) + "/";
        if (page->isDisposed()) {
            status += "d";
        }
        if (page->isEstimated()) {
            status += "e";
        }
        const QSize es = page->estimatedSize();
        status += QString::number(es.width()) + "x" + QString::number(es.height()) + ";";

        const QSize sz = page->getSourceSize();
        status += QString::number(sz.width()) + "x" + QString::number(sz.height()) + ";";

        const QSize ss = page->getScaledSize();
        status += QString::number(ss.width()) + "x" + QString::number(ss.height()) + "/";

        status += QString::number(page->numOfPages());

        QGraphicsSimpleTextItem *status_item = m_scene->addSimpleText(status, font);
        status_item->moveBy(page->x() + page->width() + 5, page->y() + 10);
        status_item->show();
    }
}

};
