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

#ifndef __JOBKEY_H
#define __JOBKEY_H

#include <QDebug>

namespace QComicBook
{
    class JobKey
    {
    public:
        JobKey();
        JobKey(int subsystem, int uniqKey);
        int getSubsystem() const;
        int getKey() const;
   
        bool operator==(const JobKey &other) const;

    private:
        int m_subsys;
        int m_key;
    };
}

QDebug operator<<(QDebug dbg, const QComicBook::JobKey &job);

#endif
