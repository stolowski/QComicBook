/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <pawel.stolowski@wp.pl>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

/*! \file PageLoaderThread.h */

#ifndef __PAGELOADERTHREAD_H
#define __PAGELOADERTHREAD_H

#include "LoaderThreadBase.h"

namespace QComicBook
{
    class Page;

    //! Thread-based image loader.
    class PageLoaderThread: public LoaderThreadBase
    {
        Q_OBJECT
            
        signals:
            void pageLoaded(const Page &);
            void pageLoaded(const Page &, const Page &);
        
        public:
            PageLoaderThread();
            virtual ~PageLoaderThread();
        
        protected:
            virtual bool process(const LoadRequest &req);
        
    };
}

#endif
