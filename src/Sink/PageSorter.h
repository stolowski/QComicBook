/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2013 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __PAGE_SORTER_H
#define __PAGE_SORTER_H

#include "FileEntry.h"

namespace QComicBook
{
    class FileEntry;

    class PageSorter
    {
    public:
        virtual bool operator()(const FileEntry& l, const FileEntry& r) const
        {
            return l.getFullFileName() < r.getFullFileName();
        }
    };
}

#endif
