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

#ifndef __LISTVIEWTHUMBNAIL_H
#define __LISTVIEWTHUMBNAIL_H

/*! \file thumbnaillistitem.h */

#include <qlistview.h>
#include "thumbnailitem.h"

namespace QComicBook
{
	class ListViewThumbnail: public QCheckListItem, public ThumbnailItem
	{
		public:
			ListViewThumbnail(QListView *parent, QListViewItem *after, int page);
			virtual ~ListViewThumbnail();
	};
}

#endif

