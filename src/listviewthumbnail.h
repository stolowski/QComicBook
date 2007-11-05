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

#ifndef __LISTVIEWTHUMBNAIL_H
#define __LISTVIEWTHUMBNAIL_H

/*! \file thumbnaillistitem.h */

#include <QListWidgetItem>
#include "thumbnailitem.h"

class QListWidget;

namespace QComicBook
{
	class ListViewThumbnail: public QListWidgetItem, public ThumbnailItem
	{
		public:
			ListViewThumbnail(QListWidget *parent, QListWidgetItem *after, int page);
			virtual ~ListViewThumbnail();
	};
}

#endif

