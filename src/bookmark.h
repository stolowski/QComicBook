/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005 Pawel Stolowski <yogin@linux.bydg.org>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

/*! \file bookmark.h */

#ifndef __BOOKMARK_H
#define __BOOKMARK_H

#include <qstring.h>

namespace QComicBook
{
	class Bookmark
	{
		private:
			QString name; //!<comic book full path
			int page; //!<page number
			int id; //!<item identifier in popup menu

		public:
			Bookmark();
			Bookmark(const QString &cbname, int cbpage);
			~Bookmark();
			const QString& getName() const;
			int getPage() const;
			int getId() const;
			void setName(const QString &cbname);
			void setPage(int cbpage);
			void setId(int n);
			bool isValid();
			QString menuItemName();
	};
}

#endif

