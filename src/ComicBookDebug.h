/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2012 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __COMICBOOKDEBUG_H
#define __COMICBOOKDEBUG_H

#include <QDebug>

#ifdef DEBUG
#define QComicBookDebug(block) { block }
#else
#define QComicBookDebug(block)
#endif

#define _DEBUG qDebug() << Q_FUNC_INFO << ':'

#endif
