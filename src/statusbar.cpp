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

#include "statusbar.h"
#include <qlabel.h>
#include <qstring.h>
#include <qimage.h>
#include <qprogressbar.h>

using namespace QComicBook;

StatusBar::StatusBar(QWidget *parent, bool elms): QStatusBar(parent)
{
	if (elms)
	{
		createInfoElements();
		setPage(0, 0);
	}
}

StatusBar::~StatusBar()
{
}

void StatusBar::createInfoElements()
{
	if (!page)
        {
                page = new QLabel(this);
                page->setFixedWidth(80);
                addWidget(page, 0);
        }
        if (!imginfo)
        {
                imginfo = new QLabel(this);
                imginfo->setFixedWidth(140);
                addWidget(imginfo, 1);
        }
        if (!name)
        {
                name = new QLabel(this);
                addWidget(name, 2);
        }
}

void StatusBar::removeInfoElements()
{
	if (page)
	{
		removeWidget(page);
		delete page;
		page = NULL;
	}
	if (imginfo)
        {
                removeWidget(imginfo);
                delete imginfo;
                imginfo = NULL;
        }
        if (name)
        {
                removeWidget(name);
                delete name;
                name = NULL;
        }
}

void StatusBar::clear()
{
	setPage(0, 0);
	setImageInfo(NULL, NULL);
	setName(QString::null);
}

void StatusBar::setPage(int n, int total)
{
	page->setText(tr("Page") + " " + QString::number(n) + " / " + QString::number(total));
}

void StatusBar::setImageInfo(const QImage *img1, const QImage *img2)
{
	QString txt;
	if (img1)
		txt = QString::number(img1->width()) + "x" + QString::number(img1->height()) + " ";
	if (img2)
		txt += QString::number(img2->width()) + "x" + QString::number(img2->height());
	imginfo->setText(txt);
}

void StatusBar::setName(const QString &n)
{
	name->setText(n);
}

void StatusBar::setProgress(int n, int total)
{
	if (n >= total)
	{
		if (pbar)
		{
			//
			// remove progressbar, create and show informative elements
			removeWidget(pbar);
			delete pbar;
			pbar = NULL;
			createInfoElements();
			page->show();
			imginfo->show();
			name->show();
		}
	}
	else
	{
		if (!pbar)
		{
			//
			// create progressbar, remove informative elements
			removeInfoElements();
			pbar = new QProgressBar(this);
			pbar->setPercentageVisible(false);
			pbar->setFixedHeight(12); //this is a bit ugly... gives chance the
			addWidget(pbar, 1);
			pbar->show();
		}
		pbar->setProgress(n, total);

	}
}

