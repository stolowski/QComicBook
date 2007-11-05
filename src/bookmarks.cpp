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

#include "bookmarks.h"
#include "cbsettings.h"
#include <QMenu>
#include <QFile>
#include <QDir>
#include <QTextStream>

using namespace QComicBook;

Bookmarks::Bookmarks(QMenu *menu): bmenu(menu), changed(false)
{
	fname = ComicBookSettings::bookmarksDir() + "/bookmarks";
}

Bookmarks::~Bookmarks()
{
}

bool Bookmarks::load()
{
	if (fname.isEmpty())
		return false;
	QFile f(fname);
	if (f.open(QIODevice::ReadOnly))
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
}

bool Bookmarks::save()
{
	if (!changed)
		return true;
	if (fname.isEmpty())
		return false;

	QFile f(fname);
	if (f.open(QIODevice::WriteOnly))
	{
		QTextStream str(&f);
		foreach (Bookmark *b, blist)
			str << b->getName() << endl << b->getPage() << endl;
		f.close();
		return true;
	}
	return false;
}

void Bookmarks::set(const QString &cbname, int page)
{
	QAction *action;
	foreach (Bookmark *b, blist)
	{
		if ((b->getName() == cbname))
		{
			if (b->getPage() == page) //same page, do nothing
				return;
			action = b->getId();
			bmenu->removeAction(action);
			bmap.remove(action);
			b->setName(cbname);
			b->setPage(page);
			action = bmenu->addAction(b->menuItemName());
			b->setId(action);
			bmap.insert(action, b);
			changed = true;
			return;
		}
	}
	Bookmark *b = new Bookmark(cbname, page);
	blist.append(b);
	action = bmenu->addAction(b->menuItemName());
	b->setId(action);
	bmap.insert(action, b);
	changed = true;
	return;
}

bool Bookmarks::remove(const QString &cbname)
{
	foreach (Bookmark *b, blist)
	{
		if ((b->getName() == cbname))
		{
			bmenu->removeAction(b->getId());
			bmap.remove(b->getId());
			int idx = blist.indexOf(b);
			if (idx >= 0)
			{
				blist.removeAt(idx);
				delete b;
			}
			return true;
		}
	}
	return false;
}

bool Bookmarks::remove(QAction *id)
{
	if (bmap.contains(id))
	{
		Bookmark *b = bmap[id];
		bmenu->removeAction(id);
		bmap.remove(id);
		int idx = blist.indexOf(b);
		if (idx >= 0)
		{
			blist.removeAt(idx);
			delete b;
		}
		changed = true;
	}
}

bool Bookmarks::get(QAction *action, Bookmark &b)
{
	if (bmap.contains(action))
	{
		b = *bmap[action];
		return true;
	}
	return false;
}

QList<Bookmark> Bookmarks::get() const
{
	QList<Bookmark> res;
	foreach (Bookmark *b, blist)
		res.push_back(*b);
	return res;
}

bool Bookmarks::exists(const QString &cbname)
{
	foreach (Bookmark *b, blist)
		if (b->getName() == cbname)
			return true;
	return false;
}

