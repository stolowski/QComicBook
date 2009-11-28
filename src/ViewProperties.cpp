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
#include <QDebug>

using namespace QComicBook;

ViewProperties::ViewProperties()
{
    setFromSettings();
}

ViewProperties::ViewProperties(const ViewProperties &props)
{
    m_size = props.m_size;
    m_angle = props.m_angle;
    m_pageNumbers = props.m_pageNumbers;
    m_contScroll = props.m_contScroll;
    m_twoPagesMode = props.m_twoPagesMode;
    m_mangaMode = props.m_mangaMode;
    m_twoPagesStep = props.m_twoPagesStep;
    m_background = props.m_background;
}

void ViewProperties::setFromSettings()
{
    ComicBookSettings &cfg(ComicBookSettings::instance());
    m_size = cfg.pageSize();
    m_angle = 0; //?
    m_pageNumbers = cfg.embedPageNumbers();
    m_contScroll = cfg.continuousScrolling();
    m_twoPagesMode = cfg.twoPagesMode();
    m_mangaMode = cfg.japaneseMode();
    m_twoPagesStep = cfg.twoPagesStep();
    m_background = cfg.background();
}

int ViewProperties::angle() const
{
    return m_angle;
}

void ViewProperties::setAngle(Rotation r, bool notify)
{
    if (r == None && m_angle == 0)
        return;
    
    if (r == Right)
        ++m_angle;
    else if (r == Left)
        --m_angle;
    else
        m_angle = 0; //None
    m_angle &= 3;
    if (notify)
    {
        emit changed();
    }
}


Size ViewProperties::size() const
{
    return m_size;
}

void ViewProperties::setSize(Size s)
{
    if (m_size != s)
    {
        m_size = s;
        emit changed();
    }
}

bool ViewProperties::pageNumbers() const
{
    return m_pageNumbers;
}

void ViewProperties::setPageNumbers(bool f)
{
    if (m_pageNumbers != f)
    {
        m_pageNumbers = f;
        emit changed();
    }
}

QColor ViewProperties::background() const
{
    return m_background;
}

void ViewProperties::setBackground(const QColor &c)
{
    if (m_background != c)
    {
        m_background = c;
        emit changed();
    }
}

bool ViewProperties::continuousScrolling() const
{
    return m_contScroll;
}

void ViewProperties::setTwoPagesMode(bool f)
{
    if (m_twoPagesMode != f)
    {
        m_twoPagesMode = f;
        emit changed();
    }
}

bool ViewProperties::twoPagesMode() const
{
    return m_twoPagesMode;
}

void ViewProperties::setTwoPagesStep(bool f)
{
    if (m_twoPagesStep != f)
    {
        m_twoPagesStep = f;
        emit changed();
    }
}

bool ViewProperties::twoPagesStep() const
{
    return m_twoPagesStep;
}

void ViewProperties::setMangaMode(bool f)
{
    if (m_mangaMode != f)
    {
        m_mangaMode = f;
        emit changed();
    }
}

bool ViewProperties::mangaMode() const
{
    return m_mangaMode;
}
