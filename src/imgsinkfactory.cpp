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

#include "imgsinkfactory.h"
#include "imgdirsink.h"
#include "imgarchivesink.h"
#include <QString>
#include <QFileInfo>

using namespace QComicBook;

ImgSinkFactory::ImgSinkFactory()
{
}

ImgSinkFactory::~ImgSinkFactory()
{
}

ImgSinkFactory& ImgSinkFactory::instance()
{
	static ImgSinkFactory f;
	return f;
}

ImgDirSink* ImgSinkFactory::createImgSink(SinkType s)
{
	if (s == ArchiveSink)
		return new ImgArchiveSink();
	if (s == DirSink)
		return new ImgDirSink();
	return NULL;
}

ImgDirSink* ImgSinkFactory::createImgSink(const QString &path)
{
	const QFileInfo finfo(path);
	if (finfo.isDir())
		return createImgSink(DirSink);
	else
		return createImgSink(ArchiveSink);
}

