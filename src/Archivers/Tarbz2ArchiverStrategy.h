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

#ifndef __TARBZ2_ARCHIVER_STRATEGY_H
#define __TARBZ2_ARCHIVER_STRATEGY_H

#include "ArchiverStrategy.h"

namespace QComicBook
{
    class Tarbz2ArchiverStrategy: public ArchiverStrategy
    {
    public:
        Tarbz2ArchiverStrategy();
        virtual ~Tarbz2ArchiverStrategy();

        virtual void configure();
    };
}

#endif
