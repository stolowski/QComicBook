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
#include <qdir.h>

QString Icons::iconpath = QString::null;
QDict<QIconSet> Icons::set;

bool Icons::init(const QString& path)/*{{{*/
{
	QDir dir(iconpath = path);
	set.setAutoDelete(true); //icons in the set will be deleted on set deletion
	return dir.exists();
}/*}}}*/

const QIconSet& Icons::get(const QString &name)/*{{{*/
{
	QIconSet *icon = set.find(name);
	if (icon)
		return *icon;
	QPixmap pixmap(iconpath + "/" + name);
	set.insert(name, icon = new QIconSet(pixmap));
	return *icon;
}/*}}}*/

