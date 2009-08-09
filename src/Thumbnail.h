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

#ifndef __THUMBNAIL_H
#define __THUMBNAIL_H

#include <QImage>
#include <QByteArray>

namespace QComicBook
{
	class Thumbnail
	{
		private:
			int num;
                        QByteArray hash;
			QImage img;
			static int thwidth, thheight; //default thumbnail width and height
                        static QByteArray getScrambledName(const QString &in);

		public:
			Thumbnail(int n, const QString &comicbookName);
			Thumbnail(int n, const QImage &i);
			~Thumbnail();

			int page() const;
			const QImage& image() const;
                        bool tryLoad();
                        bool fromOriginalImage(const QString &fname);
			bool save();
			void setImage(const QImage &i);
                        QString getFullPath() const;

			static int maxWidth();
			static int maxHeight();
	};
}

#endif

