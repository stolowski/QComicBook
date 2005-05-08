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

#include "bookmarks.h"
#include "cbsettings.h"
#include <qpopupmenu.h>
#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>

Bookmarks::Bookmarks(QPopupMenu *menu): bmenu(menu), changed(false)/*{{{*/
{
	blist.setAutoDelete(true);
	fname = ComicBookSettings::bookmarksDir() + "/bookmarks";
}/*}}}*/

Bookmarks::~Bookmarks()/*{{{*/
{
}/*}}}*/

bool Bookmarks::load()/*{{{*/
{
	if (fname.isEmpty())
		return false;
	QFile f(fname);
	if (f.open(IO_ReadOnly))
	{
		QTextStream str(&f);
		while (!str.atEnd())
		{
			QString name = str.readLine();
			if (str.atEnd())
				break;
			int page = str.readLine().toInt();
			set(name, page);
		}
		f.close();
	}
	return false;
}/*}}}*/

bool Bookmarks::save()/*{{{*/
{
	if (!changed)
		return true;
	if (fname.isEmpty())
		return false;

	QFile f(fname);
	if (f.open(IO_WriteOnly))
	{
		QTextStream str(&f);
		for (Bookmark *b = blist.first(); b; b = blist.next())
			str << b->getName() << endl << b->getPage() << endl;
		f.close();
		return true;
	}
	return false;
}/*}}}*/

void Bookmarks::set(const QString &cbname, int page)/*{{{*/
{
	int id;
	for (Bookmark *b = blist.first(); b; b = blist.next())
		if ((b->getName() == cbname))
		{
			if (b->getPage() == page) //same page, do nothing
				return;
			id = b->getId();
			bmenu->removeItem(id);
			bmap.remove(id);
			b->setName(cbname);
			b->setPage(page);
			id = bmenu->insertItem(b->menuItemName());
			b->setId(id);
			bmap.insert(id, b);
			changed = true;
			return;
		}
	Bookmark *b = new Bookmark(cbname, page);
	blist.append(b);
	id = bmenu->insertItem(b->menuItemName());
	b->setId(id);
	bmap.insert(id, b);
	changed = true;
	return;
}/*}}}*/

bool Bookmarks::remove(const QString &cbname)/*{{{*/
{
	for (Bookmark *b = blist.first(); b; b = blist.next())
		if ((b->getName() == cbname))
		{
			bmenu->removeItem(b->getId());
			bmap.remove(b->getId());
			blist.remove();
			return true;
		}
	return false;
}/*}}}*/

bool Bookmarks::remove(int id)/*{{{*/
{
	if (bmap.contains(id))
	{
		Bookmark *b = bmap[id];
		bmenu->removeItem(id);
		bmap.remove(id);
		blist.remove(b);
		changed = true;
	}
}/*}}}*/

bool Bookmarks::get(int id, Bookmark &b)/*{{{*/
{
	if (bmap.contains(id))
	{
		b = *bmap[id];
		return true;
	}
	return false;
}/*}}}*/

QValueList<Bookmark> Bookmarks::get()/*{{{*/
{
	QValueList<Bookmark> res;
	for (Bookmark *b = blist.first(); b; b = blist.next())
		res.push_back(*b);
	return res;
}/*}}}*/

bool Bookmarks::exists(const QString &cbname)/*{{{*/
{
	for (Bookmark *b = blist.first(); b; b = blist.next())
		if (b->getName() == cbname)
			return true;
	return false;
}/*}}}*/

