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
#include "Page.h"
#include <QImage>
#include <QDebug>

using namespace QComicBook;

ImgSink::ImgSink(int cacheSize): QObject()
{
	cache = new ImgCache(cacheSize);
}

ImgSink::~ImgSink()
{
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
		qDebug() << "from cache:" << num;
	}
	else
	{
		im = image(num, result); //TODO check result
		cache->insertImage(num, im);
		qDebug() << "to cache:" << num;
	}
	const Page page(num, im);
	return page;
}

