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

#ifndef __BOOKMARK_H
#define __BOOKMARK_H

#include <qstring.h>

class Bookmark
{
	private:
		QString name;
		int page;
		int id;

	public:
		Bookmark() {}
		Bookmark(const QString &cbname, int cbpage): name(cbname), page(cbpage), id(0) {};
		~Bookmark() {};
		const QString& getName() const { return name; }
		int getPage() const { return page; }
		int getId() const { return id; }
		void setName(const QString &cbname) { name = cbname; }
		void setPage(int cbpage) { page = cbpage; }
		void setId(int n) { id = n; }
		QString menuItemName() { return name + " [" + QString::number(page + 1) + "]"; }
};

#endif

