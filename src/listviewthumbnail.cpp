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

#include "listviewthumbnail.h"

using namespace QComicBook;

ListViewThumbnail::ListViewThumbnail(QListView *parent, QListViewItem *after, int page): QCheckListItem(parent, after, QString::number(page+1), QCheckListItem::CheckBox), ThumbnailItem(page)
{
}

ListViewThumbnail::~ListViewThumbnail()
{
}

