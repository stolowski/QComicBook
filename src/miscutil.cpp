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

#include "miscutil.h"
#include <qstringlist.h>
#include <qfileinfo.h>
#include <stdlib.h>

bool which(const QString &command)/*{{{*/
{
	QString paths = QString(getenv("PATH"));
	QStringList plist = QStringList::split(":", paths, false);
	for (QStringList::const_iterator it = plist.begin(); it != plist.end(); it++)
	{
		QFileInfo finfo(*it + "/" + command);
		if (finfo.isExecutable())
				return true;
	}
	return false;
}/*}}}*/

