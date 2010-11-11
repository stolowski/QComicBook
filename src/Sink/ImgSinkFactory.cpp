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

#include "ImgSinkFactory.h"
#include "ImgPdfSink.h"
#include "ImgDirSink.h"
#include "ImgArchiveSink.h"
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

void ImgSinkFactory::deleteLater(ImgSink *sink)
{
	sink->deleteLater();
}

QSharedPointer<ImgSink> ImgSinkFactory::createImgSink(SinkType s)
{
	if (s == ArchiveSink)
		return QSharedPointer<ImgSink>(new ImgArchiveSink(), ImgSinkFactory::deleteLater);
	if (s == DirSink)
		return QSharedPointer<ImgSink>(new ImgDirSink(), ImgSinkFactory::deleteLater);
	if (s == PdfSink)
		return QSharedPointer<ImgSink>(new ImgPdfSink(), ImgSinkFactory::deleteLater);
	return QSharedPointer<ImgSink>();
}

QSharedPointer<ImgSink> ImgSinkFactory::createImgSink(const QString &path)
{
	const QFileInfo finfo(path);
	if (finfo.isDir())
		return createImgSink(DirSink);
	else if (path.endsWith("pdf")) //FIXME
		return createImgSink(PdfSink);
	else
		return createImgSink(ArchiveSink);
}

