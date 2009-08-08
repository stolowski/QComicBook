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

#include "Bookmark.h"
#include <QFileInfo>
#include <QAction>

using namespace QComicBook;

Bookmark::Bookmark()
{
}

Bookmark::Bookmark(const QString &cbname, int cbpage): name(cbname), page(cbpage), id(NULL)
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

QAction* Bookmark::getId() const
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

void Bookmark::setId(QAction *n)
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

