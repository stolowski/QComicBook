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

#include "thumbnailitem.h"

ThumbnailItem::ThumbnailItem(QIconView *view, int page, const QPixmap &pixmap): QIconViewItem(view, QString::number(page+1), pixmap), pnum(page), loaded(false)/*{{{*/
{
}/*}}}*/

ThumbnailItem::~ThumbnailItem()/*{{{*/
{
}/*}}}*/

int ThumbnailItem::page() const/*{{{*/
{
	return pnum;
}/*}}}*/

void ThumbnailItem::setLoaded(bool f)/*{{{*/
{
	loaded = f;
}/*}}}*/

bool ThumbnailItem::isLoaded() const/*{{{*/
{
	return loaded;
}/*}}}*/

