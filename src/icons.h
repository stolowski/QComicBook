/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <yogin@linux.bydg.org>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __COMICBOOKICONS_H
#define __COMICBOOKICONS_H

#include <qiconset.h>
#include <qdict.h>

namespace QComicBook
{
	class Icons
	{
		private:
			static QDict<QIconSet> set;
			static QString iconpath;

		public:
			static bool init(const QString &path);
			static const QIconSet& get(const QString &name);
	};
}

#endif

