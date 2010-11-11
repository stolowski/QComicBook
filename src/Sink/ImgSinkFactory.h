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

#ifndef __IMGSINKFACTORY_H
#define __IMGSINKFACTORY_H

/*! \file imgsinkfactory.h */

#include <QSharedPointer>

class QString;
	
namespace QComicBook
{
	enum SinkType
	{
		ArchiveSink = 1,
		DirSink,
		PdfSink
	};

	class ImgSink;

	class ImgSinkFactory
	{
		private:
			ImgSinkFactory();
			~ImgSinkFactory();

		public:
			static ImgSinkFactory& instance();
			static void deleteLater(ImgSink *sink);
			QSharedPointer<ImgSink> createImgSink(SinkType s);
			QSharedPointer<ImgSink> createImgSink(const QString &path);
	};
}

#endif

