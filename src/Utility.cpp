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

#include "Utility.h"
#include <QCoreApplication>
#include <QStringList>
#include <QFileInfo>
#include <stdlib.h>
#include <utime.h>

QString Utility::which(const QString &command)
{
	const QString paths = QString(getenv("PATH"));
#ifdef Q_WS_WIN
	QStringList plist = paths.split(";");
	plist.prepend(QCoreApplication::applicationDirPath());
#else
	QStringList plist = paths.split(":");
#endif
	for (QStringList::const_iterator it = plist.begin(); it != plist.end(); it++)
	{
#ifdef Q_WS_WIN
		QFileInfo finfo(*it + "/" + command + ".exe");
#else
		QFileInfo finfo(*it + "/" + command);
#endif
		if (finfo.isExecutable())
			return finfo.absoluteFilePath();
	}
	return QString::null;
}

void Utility::touch(const QString &fname)
{
    utime(fname.toLocal8Bit(), NULL);
}
