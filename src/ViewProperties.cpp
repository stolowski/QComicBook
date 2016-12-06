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

#include "ViewProperties.h"
#include "ComicBookSettings.h"

using namespace QComicBook;

ViewProperties::ViewProperties()
{
    setFromSettings();
}

ViewProperties::ViewProperties(const ViewProperties &props)
{
    m_data = props.m_data;
}

void ViewProperties::setFromSettings()
{
    ComicBookSettings &cfg(ComicBookSettings::instance());
    m_data.size = cfg.pageSize();
    m_data.angle = 0; //?
    m_data.pageNumbers = cfg.embedPageNumbers();
    m_data.contScroll = cfg.continuousScrolling();
    m_data.twoPagesMode = cfg.twoPagesMode();
    m_data.mangaMode = cfg.japaneseMode();
    m_data.background = cfg.background();
    m_data.smoothScaling = cfg.smoothScaling();
    m_data.gapSize = cfg.gapSize();
}

int ViewProperties::angle() const
{
    return m_data.angle;
}

void ViewProperties::setAngle(Rotation r, bool notify)
{
    if (r == None && m_data.angle == 0)
        return;
    
    if (r == Right)
        ++m_data.angle;
    else if (r == Left)
        --m_data.angle;
    else
        m_data.angle = 0; //None
    m_data.angle &= 3;
    if (notify)
    {
        emit changed();
    }
}


Size ViewProperties::size() const
{
    return m_data.size;
}

void ViewProperties::setSize(Size s)
{
    if (m_data.size != s)
    {
        m_data.size = s;
        emit changed();
    }
}

bool ViewProperties::pageNumbers() const
{
    return m_data.pageNumbers;
}

void ViewProperties::setPageNumbers(bool f)
{
    if (m_data.pageNumbers != f)
    {
        m_data.pageNumbers = f;
        emit changed();
    }
}

QColor ViewProperties::background() const
{
    return m_data.background;
}

void ViewProperties::setBackground(const QColor &c)
{
    if (m_data.background != c)
    {
        m_data.background = c;
        emit changed();
    }
}

bool ViewProperties::continuousScrolling() const
{
    return m_data.contScroll;
}

void ViewProperties::setTwoPagesMode(bool f)
{
    if (m_data.twoPagesMode != f)
    {
        m_data.twoPagesMode = f;
        emit changed();
    }
}

bool ViewProperties::twoPagesMode() const
{
    return m_data.twoPagesMode;
}

void ViewProperties::setMangaMode(bool f)
{
    if (m_data.mangaMode != f)
    {
        m_data.mangaMode = f;
        emit changed();
    }
}

bool ViewProperties::mangaMode() const
{
    return m_data.mangaMode;
}

const ViewPropertiesData& ViewProperties::getProperties() const
{
    return m_data;
}

void ViewProperties::setGapSize(int f)
{
    if (m_data.gapSize != f)
        {
            m_data.gapSize = f;
            emit changed();
        }
}
int ViewProperties::gapSize() const
{
    return m_data.gapSize;
}