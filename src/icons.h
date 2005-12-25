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

#ifndef __COMICBOOKICONS_H
#define __COMICBOOKICONS_H

#include <qiconset.h>
#include <qdict.h>
#include <qmap.h>
#include <kicontheme.h>

namespace QComicBook
{
	enum IconStyle { 
		QComicBookIcons = 0,
		KDEDefaultIcons
	};

	class Icons
	{
		private:
			static QDict<QIconSet> set;
			static QMap<QString, QString> namemap;
			static IconStyle istyle;

		public:
			static bool init(IconStyle style = QComicBookIcons);
			static const QIconSet& get(const QString &name, KIcon::Group group = KIcon::NoGroup);
	};
}

#endif

