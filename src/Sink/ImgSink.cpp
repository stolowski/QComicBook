/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "ImgSink.h"
#include "ImgCache.h"
#include "../Page.h"
#include "Thumbnail.h"
#include <QImage>
#include "../ComicBookDebug.h"

using namespace QComicBook;

ImgSink::ImgSink(int cacheSize): cbname(QString()), cbfullname(QString()), QObject()
{
	cache = new ImgCache(cacheSize);
}

ImgSink::~ImgSink()
{
    _DEBUG;
    delete cache;
}

void ImgSink::setCacheSize(int cacheSize, bool autoAdjust)
{
	cache->setSize(cacheSize, autoAdjust);
}

Page ImgSink::getImage(unsigned int num, int &result)
{
	QImage im;
	if (cache->get(num, im))
	{
		result = 0;
		_DEBUG << "from cache:" << num;
	}
	else
	{
		im = image(num, result); //TODO check result
		cache->insertImage(num, im);
		_DEBUG << "to cache:" << num;
	}
	const Page page(num, im);
	return page;
}

Thumbnail ImgSink::getThumbnail(unsigned int num, bool thumbcache)
{
        Thumbnail t(num, cbname.remove('/'));

        //
        // try to load cached thumbnail
        if (thumbcache)
        {
            if (t.tryLoad())
            {
                _DEBUG << "thumbnail" << num << "loaded from disk";
                return t;
            }
        }

		int result;
        //
        // try to load image
        const Page p(getImage(num, result));
		if (result == 0)
        {
			t.setImage(p.getImage());
            //
            // save thumbnail if caching enabled
            if (thumbcache)
            {
                t.save();
            }
        }

        return t;
}


void ImgSink::setComicBookName(const QString &name, const QString &fullName)
{
	cbname = name;
	cbfullname = fullName;
}

QString ImgSink::getFullName() const
{
	return cbfullname;
}

QString ImgSink::getName(int maxlen) const
{
	if (cbname.length() < maxlen)
		return cbname;
	const QString tmpname(cbname.left(maxlen-3) + "...");
	return tmpname;
}

