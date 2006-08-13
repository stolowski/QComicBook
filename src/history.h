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

#ifndef __HISTORY_H
#define __HISTORY_H

#include <qstringlist.h>

namespace Utility
{
	class History
	{
		private:
			QStringList hlist;
			QString key;
			int size;
		public:
			History(const QStringList &l, int max = 10);
			History(int max = 10);
			virtual ~History();
			void remove(const QString &txt);
			void append(const QString &txt);
			void set(const QStringList &l);
			QString first();
			QStringList getAll() const;
	};
}

#endif

