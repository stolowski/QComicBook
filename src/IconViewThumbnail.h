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

#ifndef __ICONVIEWTHUMBNAIL_H
#define __ICONVIEWTHUMBNAIL_H

/*! \file iconviewthumbnail.h */

#include <QListWidgetItem>
#include "ThumbnailItem.h"

namespace QComicBook
{
	class IconViewThumbnail: public QListWidgetItem, public ThumbnailItem
	{
		public:
			IconViewThumbnail(QListWidget *view, int page, const QPixmap &pixmap);
			virtual ~IconViewThumbnail();
	};
}

#endif

