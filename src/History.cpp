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

#include "history.h"

using namespace Utility;

History::History(const QStringList &l, int max): size(max)
{
	hlist = l;
}

History::History(int max)
{
	size = max;
}

History::~History()
{
}

void History::append(const QString &txt)
{
	const int i = hlist.indexOf(txt);
	
	//
	// text not found on the list - append it
	if (i < 0)
	{
		hlist.push_front(txt);
		//
		// remove last text item if history reached its max size
		if (hlist.count() > size)
			hlist.pop_back();
	}
	else //put the item to the front
	{
		hlist.removeAt(i);
		hlist.push_front(txt);
	}
}

void History::remove(const QString &txt)
{
	const int i = hlist.indexOf(txt);
	if (i >= 0)
		hlist.removeAt(i);
}

void History::set(const QStringList &l)
{
	hlist = l;
}

QString History::first()
{
	return hlist.first(); 
}

QStringList History::getAll() const
{
	return hlist;
}

