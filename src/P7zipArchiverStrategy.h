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

#ifndef __P7ZIP_ARCHIVER_STRATEGY_H
#define __P7ZIP_ARCHIVER_STRATEGY_H

#include "ArchiverStrategy.h"

namespace QComicBook
{
    class P7zipArchiverStrategy: public ArchiverStrategy
    {
    public:
        P7zipArchiverStrategy();
        virtual ~P7zipArchiverStrategy();

        virtual void configure();
    };
}

#endif
