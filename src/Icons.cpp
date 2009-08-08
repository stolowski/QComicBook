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

#include "Icons.h"
#include <QDir>

using namespace QComicBook;

QString Icons::iconpath = QString::null;
QHash<QString, QIcon*> Icons::set;

bool Icons::init(const QString& path)
{
	QDir dir(iconpath = path);
	//set.setAutoDelete(true); //icons in the set will be deleted on set deletion TODO
	return dir.exists();
}

const QIcon& Icons::get(const QString &name)
{
	QHash<QString, QIcon *>::iterator it = set.find(name);
	if (it != set.end())
		return **it;
	QIcon *icon;
	QPixmap pixmap(iconpath + "/" + name);
	set.insert(name, icon = new QIcon(pixmap));
	return *icon;
}

