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

#ifndef __COMICBOOKICONS_H
#define __COMICBOOKICONS_H

#include <QIcon>
#include <QHash>

namespace QComicBook
{
	class Icons
	{
		private:
			static QHash<QString, QIcon*> set;
			static QString iconpath;

		public:
			static bool init(const QString &path);
			static const QIcon& get(const QString &name);
	};
}

#endif

