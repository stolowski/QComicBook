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

#include "ThumbnailLoaderThread.h"
#include "ImgDirSink.h"
#include "Thumbnail.h"
#include <QDebug>
 
using namespace QComicBook;

ThumbnailLoaderThread::ThumbnailLoaderThread(bool cache): LoaderThreadBase(), usecache(cache)
{
}

ThumbnailLoaderThread::~ThumbnailLoaderThread()
{
}

bool ThumbnailLoaderThread::process(const LoadRequest &req)
{
    if (req.twoPages)
    {                
        qWarning() << "thumbnail request for two pages";
    }
    else
    {
        qDebug() << "thumbnail requested: " << req.pageNumber;
        Thumbnail *t = sink->getThumbnail(req.pageNumber, usecache);
        if (t)
        {
            emit thumbnailLoaded(t);
        }
    }
    return true;
}

void ThumbnailLoaderThread::setUseCache(bool f)
{
    mtx.lock();
    usecache = f;
    mtx.unlock();
}


