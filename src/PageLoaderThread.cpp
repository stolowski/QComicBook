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

#include "PageLoaderThread.h"
#include "Page.h"
#include "Sink/ImgDirSink.h"

using namespace QComicBook;

PageLoaderThread::PageLoaderThread(): LoaderThreadBase()
{
}

PageLoaderThread::~PageLoaderThread()
{
}

bool PageLoaderThread::process(const LoadRequest &req)
{
    int result;
    if (req.twoPages)
    {                
        const Page page1(sink->getImage(req.pageNumber, result));
        const Page page2(sink->getImage(req.pageNumber+1, result));
        emit pageLoaded(page1, page2); //TODO errors
    }
    else
    {
        const Page page(sink->getImage(req.pageNumber, result));
        emit pageLoaded(page);
    }
    return true;
}
