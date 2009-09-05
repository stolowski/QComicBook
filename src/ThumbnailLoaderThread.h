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

/*! \file thumbnailloader.h */

#ifndef __THUMBNAILLOADER_H
#define __THUMBNAILLOADER_H

#include "LoaderThreadBase.h"
#include <QMutex>

namespace QComicBook
{
    class Thumbnail;

    class ThumbnailLoaderThread: public LoaderThreadBase
    {
    Q_OBJECT
            
    signals:
        void thumbnailLoaded(Thumbnail *);

    public:
        ThumbnailLoaderThread(bool cache=false);
        virtual ~ThumbnailLoaderThread();
        virtual void setUseCache(bool f);
        
    protected:
        virtual bool process(const LoadRequest &req);
        
        
    private:
        QMutex mtx;
        volatile bool usecache;
    };
}

#endif

