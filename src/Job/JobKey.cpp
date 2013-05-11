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

#include "JobKey.h"

using namespace QComicBook;

JobKey::JobKey(): m_subsys(-1), m_key(-1)
{
}

JobKey::JobKey(int subsystem, int uniqKey): m_subsys(subsystem), m_key(uniqKey)
{
}

int JobKey::getSubsystem() const 
{
    return m_subsys;
}

int JobKey::getKey() const
{
    return m_key;
}

bool JobKey::operator==(const JobKey &other) const
{
    return m_subsys == other.m_subsys && m_key == other.m_key;
}

QDebug operator<<(QDebug dbg, const QComicBook::JobKey &key)
{
    dbg.nospace() << key.getSubsystem() << ":" << key.getKey();
    return dbg.space();
}
