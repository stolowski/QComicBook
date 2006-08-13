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

#include "bookmark.h"
#include <qfileinfo.h>

using namespace QComicBook;

Bookmark::Bookmark()
{
}

Bookmark::Bookmark(const QString &cbname, int cbpage): name(cbname), page(cbpage), id(0)
{
}

Bookmark::~Bookmark()
{
}
		
const QString& Bookmark::getName() const
{ 
	return name;
}

int Bookmark::getPage() const 
{
	return page; 
}

int Bookmark::getId() const
{
	return id; 
}

void Bookmark::setName(const QString &cbname)
{
	name = cbname; 
}

void Bookmark::setPage(int cbpage)
{
	page = cbpage;
}

void Bookmark::setId(int n)
{
	id = n;
}

bool Bookmark::isValid()
{
	QFileInfo inf(name); return inf.exists();
}

QString Bookmark::menuItemName()
{
	return name + " [" + QString::number(page + 1) + "]";
}

