/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <yogin@linux.bydg.org>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "thumbnailsview.h"
#include "iconviewthumbnail.h"
#include "thumbnail.h"
#include <qpixmap.h>
#include <qstring.h>
#include <qpainter.h>
#include <QVector>
#include <QMenu>

using namespace QComicBook;

ThumbnailsView::ThumbnailsView(QWidget *parent): QListWidget(parent), selected(NULL), numpages(0)
{
	//setFocusPolicy(QWidget::NoFocus);
	setDragDropMode(QAbstractItemView::NoDragDrop);
	setMovement(QListView::Static);
	//arrangeItemsInGrid(true);
	setViewMode(QListView::IconMode);
	setIconSize(QSize(Thumbnail::maxWidth(), Thumbnail::maxHeight()));
	setResizeMode(QListView::Adjust);
	//setAutoArrange(true);
	//setResizeMode(QIconView::Adjust);
	//setMaxItemWidth(Thumbnail::maxWidth());

	//
	// context menu
	menu = new QMenu(this);
	menu->addAction(tr("Go to"), this, SLOT(goToPageAction()));
	
	//
	// create "empty page" image
	emptypage = new QPixmap(Thumbnail::maxWidth(), Thumbnail::maxHeight());
	emptypage->fill(Qt::white);
	QPainter paint(emptypage);
	QPen pen(Qt::black, 3);
	paint.setPen(pen);
	paint.drawRect(0, 0, Thumbnail::maxWidth(), Thumbnail::maxHeight());

	connect(this, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onDoubleClick(QListWidgetItem *)));
	connect(this, SIGNAL(contextMenuRequested(QListWidgetItem *, const QPoint&)), this, SLOT(showContextMenu(QListWidgetItem *, const QPoint&)));
}

ThumbnailsView::~ThumbnailsView()
{
	delete emptypage;
}

void ThumbnailsView::onDoubleClick(QListWidgetItem *item)
{
	emit requestedPage(dynamic_cast<ThumbnailItem *>(item)->page(), false);
}

void ThumbnailsView::setPages(int pages)
{
	clear();

	icons.resize(numpages = pages);	
	for (int i=0; i<numpages; i++)
		icons.insert(i, new IconViewThumbnail(this, i, *emptypage));

	//setArrangement(visibleWidth() > visibleHeight() ? QIconView::LeftToRight : QIconView::TopToBottom);
}

void ThumbnailsView::setPage(int n, const QPixmap &img)
{
	if (n < icons.count())
	{
		IconViewThumbnail *th = icons[n];
		th->setIcon(img);
		th->setLoaded(true);
	}
}

void ThumbnailsView::setPage(const Thumbnail &t)
{
	setPage(t.page(), QPixmap::fromImage(t.image()));
}

void ThumbnailsView::clear()
{
	QListWidget::clear();
	icons.clear();
	numpages = 0;
	selected = NULL;
}

void ThumbnailsView::scrollToPage(int n)
{
	if (n < icons.count())
	{
		IconViewThumbnail *th = icons[n];
		setCurrentItem(th);
		if (isVisible())
			scrollToItem(th);
			//ensureVisible(th->x(), th->y());
	}
}

void ThumbnailsView::showContextMenu(QListWidgetItem *item, const QPoint &p)
{
	if (item)
	{
		selected = item;
		menu->popup(p);
	}
}

void ThumbnailsView::goToPageAction()
{
	if (selected)
		onDoubleClick(selected);
}

bool ThumbnailsView::isLoaded(int n) const
{
	return (n < icons.count()) ? icons[n]->isLoaded() : false;
}

