/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2011 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include <QImage>
#include <QPainter>
#include "../ComicBookDebug.h"
#include <algorithm>
#include "PageRedrawJob.h"
#include "../Page.h"
#include "PageWidget.h"

using namespace QComicBook;

PageRedrawJob::PageRedrawJob(): ImageTransformJob(), m_result(0)
{
    m_image[0] = m_image[1] = 0;
}

PageRedrawJob::~PageRedrawJob()
{
    _DEBUG;
    delete m_result;
    delete m_image[0];
    delete m_image[1];
}

void PageRedrawJob::setImage(const Page &p1)
{
    m_image[0] = new QImage(p1.getImage());
    m_numbers[0] = p1.getNumber();
}

void PageRedrawJob::setImage(const Page &p1, const Page &p2)
{
    m_image[0] = new QImage(p1.getImage());
    m_image[1] = new QImage(p2.getImage());

    m_numbers[0] = p1.getNumber();
    m_numbers[1] = p2.getNumber();
}

void PageRedrawJob::execute()
{
    _DEBUG;

    delete m_result;
    m_result = new QImage(m_width, m_height, QImage::Format_RGB32);

    QPainter p(m_result);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true /*ComicBookSettings::instance().smoothScaling()*/);

    p.setWorldMatrix(*m_matrix);
    p.setWorldMatrixEnabled(true);

    if (m_image[1]) // 2 pages mode
    {
	const int swap(m_props.mangaMode);
        // clear areas not covered by page (if pages sizes differ)
        for (int i=0; i<2; i++)
        {
            const int j(i^swap);
            if (m_image[j]->height() < std::max(m_image[0]->height(), m_image[1]->height()))
            {
                p.fillRect(i*m_image[j]->width(), m_image[j]->height(), m_image[j]->width(), m_sourceSize.height() - m_image[j]->height(), m_props.background);
                break; //only one page may be smaller
            }
        }

        p.drawImage(0, 0, *m_image[0^swap], 0, 0);
        p.drawImage(m_image[0^swap]->width(), 0, *m_image[1^swap], 0, 0);
        if (m_props.pageNumbers)
        {
            p.setWorldMatrixEnabled(false);
            drawPageNumber(std::max(m_numbers[swap], m_numbers[1^swap]), p, m_width, m_height);
        }
    }
    else // 1 page mode
    {
        p.drawImage(0, 0, *m_image[0], 0, 0);
        if (m_props.pageNumbers)
        {
            p.setWorldMatrixEnabled(false);
            drawPageNumber(m_numbers[0], p, m_width, m_height);
        }
    }
    p.end();
}

void PageRedrawJob::drawPageNumber(int page, QPainter &p, int x, int y)
{
    const QString pagestr(QString::number(page + 1));
    const QFontMetrics mtr(p.fontMetrics());
    const int txtw(mtr.width(pagestr));
    p.setPen(Qt::black);
    p.fillRect(x - txtw - 5, y - 2 - mtr.height(), txtw + 5, mtr.height() + 4, Qt::white);
    p.drawText(x - txtw - 4, y - 4, pagestr);
}

void PageRedrawJob::setViewProperties(const ViewPropertiesData &props)
{
    m_props = props;
}

void PageRedrawJob::setSourceSize(const QSize &size)
{
    m_sourceSize = size;
}

QImage PageRedrawJob::getResult() const
{
    if (m_result)
    {
        return *m_result;
    }
    return QImage();
}
