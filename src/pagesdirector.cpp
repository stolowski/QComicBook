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

#include "pagesdirector.h"
#include "imgdirsink.h"
#include "thumbnail.h"
#include "thumbnailevent.h"

using namespace QComicBook;

PagesDirector::PagesDirector(QWidget *parent, ImgDirSink *snk): QListView(parent), sink(snk)
{
	const int n = sink->numOfImages();
	pages = new QCheckListItem* [n];

	addColumn(tr("Page Number"));
	addColumn(tr("Image"));
	
	setColumnAlignment(0, Qt::AlignCenter);
	setColumnAlignment(1, Qt::AlignVCenter | Qt::AlignLeft);
	setSorting(-1);

	for (int i=0; i<n; i++)
	{
		pages[i] = new QCheckListItem(this, i>0 ? pages[i-1] : NULL, QString::number(i+1), QCheckListItem::CheckBox);
		pages[i]->setOn(true);
	}

	loader = new ThumbnailLoaderThread();
	loader->setSink(sink);
	loader->setReciever(this);
	loader->setUseCache(false);
	loader->request(0, n);
}

PagesDirector::~PagesDirector()
{
	loader->stop();
	loader->wait();
	delete [] pages;
}

void PagesDirector::customEvent(QCustomEvent *e)
{
	if (e->type() == ThumbnailReady)
	{
		ThumbnailEvent *evt = dynamic_cast<ThumbnailEvent *>(e);
		const Thumbnail *thumb = evt->getThumbnail();
		const int p = thumb->page();
		if (p < sink->numOfImages())
		{
			pages[p]->setPixmap(1, thumb->image());
		}
	}
	else
		QListView::customEvent(e);
}

QValueList<int> PagesDirector::markedPages()
{
	QValueList<int> marked;
	const int n = sink->numOfImages();
	for (int i=0; i<n; i++)
		if (pages[i]->isOn())
			marked.append(i);
	return marked;
}

void PagesDirector::moveUp(QListViewItem *item)
{
	QListViewItem *above = item->itemAbove();
	if (above)
		above = above->itemAbove();
	if (above)
	{
		item->moveItem(above);
	}
	else
	{
		takeItem(item);
		insertItem(item);
		setSelected(item, true);
	}
	ensureItemVisible(item);
}

void PagesDirector::moveUpSelected()
{
	if (QListViewItem *item = selectedItem())
		moveUp(item);
}

void PagesDirector::moveDown(QListViewItem *item)
{
	QListViewItem *below = item->itemBelow();
	if (below)
		item->moveItem(below);
	ensureItemVisible(item);
}

void PagesDirector::moveDownSelected()
{
	if (QListViewItem *item = selectedItem())
		moveDown(item);
}

