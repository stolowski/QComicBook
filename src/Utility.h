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

#ifndef __MISCUTIL_H
#define __MISCUTIL_H

class QString;

namespace Utility
{
	QString which(const QString &command); //similiar to shell 'which' command
        void touch(const QString &path);
        QString shortenPath(const QString &path, const QString &filler, int maxlen);
}

#endif

