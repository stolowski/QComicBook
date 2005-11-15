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

#include "icons.h"
#include <kstandarddirs.h>
#include <qdir.h>

using namespace QComicBook;

QDict<QIconSet> Icons::set;

bool Icons::init()
{
	set.setAutoDelete(true); //icons in the set will be deleted on set deletion
	return true;
}

const QIconSet& Icons::get(const QString &name)
{
	QIconSet *icon = set.find(name);
	if (icon)
		return *icon;
	QPixmap pixmap(locate("appdata", name));
	set.insert(name, icon = new QIconSet(pixmap));
	return *icon;
}

