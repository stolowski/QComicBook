/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2012 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "StatusBar.h"
#include "Page.h"
#include <QLabel>
#include <QProgressBar>

using namespace QComicBook;

StatusBar::StatusBar(QWidget *parent, bool elms): QStatusBar(parent), page(NULL), imginfo(NULL), name(NULL), pbar(NULL)
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
        page->setMinimumWidth(120);
        addWidget(page, 0);
    }
    if (!imginfo)
    {
        imginfo = new QLabel(this);
        imginfo->setMinimumWidth(140);
        addWidget(imginfo, 1);
    }
    if (!name)
    {
        name = new QLabel(this);
        addWidget(name, 2);
    }
    page->show();
    imginfo->show();
    name->show();
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

void StatusBar::createProgressBar()
{
    if (!pbar)
	{
        pbar = new QProgressBar(this);
		//pbar->setPercentageVisible(false);
		pbar->setFixedHeight(12); //this is a bit ugly... gives chance the
		addWidget(pbar, 1);
		pbar->show();
	}
}

void StatusBar::removeProgressBar()
{
	if (pbar)
	{
		removeWidget(pbar);
		delete pbar;
		pbar = NULL;
	}
}

void StatusBar::setShown(bool v)
{
    if(v)
        show();
    else
        hide();
}

void StatusBar::clear()
{
	setPage(0, 0);
	setImageInfo(NULL, NULL);
	setName(QString());
}

void StatusBar::setPage(int n, int total)
{
    if (!page)
    {
        removeProgressBar();
		createInfoElements();
	}
	page->setText(tr("Page") + " " + QString::number(n) + " / " + QString::number(total));
}

void StatusBar::setImageInfo(const Page *img1, const Page *img2)
{
	if (!imginfo)
	{
		removeProgressBar();
		createInfoElements();
	}
	QString txt;
	if (img1)
            txt = QString::number(img1->getImage().width()) + "x" + QString::number(img1->getImage().height()) + " ";
	if (img2)
            txt += QString::number(img2->getImage().width()) + "x" + QString::number(img2->getImage().height());
	imginfo->setText(txt);
}

void StatusBar::setName(const QString &n)
{
	if (!name)
	{
		removeProgressBar();
		createInfoElements();
	}
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
			removeProgressBar();
			createInfoElements();
		}
	}
	else
	{
		if (!pbar)
		{
			//
			// create progressbar, remove informative elements
			removeInfoElements();
			createProgressBar();
		}
		//pbar->setProgress(n, total);
		pbar->setMaximum(total);
		pbar->setValue(n);

	}
}

