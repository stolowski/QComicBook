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

/*! \file bookmark.h */

#ifndef __BOOKMARK_H
#define __BOOKMARK_H

#include <QString>

class QAction;

namespace QComicBook
{
	class Bookmark
	{
		private:
			QString name; //!<comic book full path
			int page; //!<page number
			QAction *id; //!<item identifier in popup menu

		public:
			Bookmark();
			Bookmark(const QString &cbname, int cbpage);
			~Bookmark();
			const QString& getName() const;
			int getPage() const;
			QAction* getId() const;
			void setName(const QString &cbname);
			void setPage(int cbpage);
			void setId(QAction *n);
			bool isValid();
			QString menuItemName();
	};
}

#endif

