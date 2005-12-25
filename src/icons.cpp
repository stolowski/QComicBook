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
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qfile.h>

using namespace QComicBook;

QDict<QIconSet> Icons::set;
QMap<QString, QString> Icons::namemap;
IconStyle Icons::istyle;

bool Icons::init(IconStyle style)
{
	istyle = style;
	const QString &iconsfile = KGlobal::dirs()->findResource("appdata", "icons.cfg");
	if (iconsfile != QString::null)
	{
		QFile f(iconsfile);
		if (!f.open(IO_ReadOnly))
			return false;
		QTextStream str(&f);
		while (!f.atEnd())
		{
			QString key, val[2];
			str >> key >> val[0] >> val[1];
			namemap.insert(key, val[static_cast<int>(style)]);

		}
		f.close();
	}
	set.setAutoDelete(true); //icons in the set will be deleted on set deletion
	return true;
}

const QIconSet& Icons::get(const QString &name, KIcon::Group group)
{
	const QString fname = namemap.contains(name) ? namemap[name] : name;
	QIconSet *icon = set.find(name);
	if (icon)
		return *icon;
	QPixmap pixmap = KGlobal::iconLoader()->loadIcon(fname, istyle == QComicBookIcons ? KIcon::User : group, 0, KIcon::DefaultState, 0L, true);
	set.insert(name, icon = new QIconSet(pixmap));
	return *icon;
}

