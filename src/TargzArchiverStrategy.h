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

#ifndef __TARGZ_ARCHIVER_STRATEGY_H
#define __TARGZ_ARCHIVER_STRATEGY_H

#include "ArchiverStrategy.h"

namespace QComicBook
{
    class TargzArchiverStrategy: public ArchiverStrategy
    {
    public:
        TargzArchiverStrategy();
        virtual ~TargzArchiverStrategy();

        virtual void configure();
    };
}

#endif
