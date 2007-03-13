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

/*! \file bookmarks.h */

#ifndef __BOOKMARKS_H
#define __BOOKMARKS_H

#include <QString>
#include <QList>
#include <QMap>
#include "bookmark.h"

class QMenu;

namespace QComicBook
{
	class Bookmarks
	{
		private:
			QList<Bookmark *> blist; //!<pointers to bookmark objects
			QMap<int, Bookmark *> bmap; //!<for fast id->bookmark lookup
			QMenu *bmenu; //!<popup menu containing bookmarks
			bool changed; //!<flag indicator to decide if bookmarks should be saved
			QString fname; //!<bookmarks file

		public:
			Bookmarks(QMenu *menu);
			~Bookmarks();
			bool load();
			bool save();

			void set(const QString &cbname, int page);
			bool remove(const QString &cbname);
			bool remove(int id);
			bool get(int id, Bookmark &b);
			QList<Bookmark> get();
			bool exists(const QString &cbname);
	};
}

#endif

