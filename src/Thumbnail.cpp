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

#include "Thumbnail.h"
#include <utime.h>

using namespace QComicBook;

int Thumbnail::thwidth = 100;
int Thumbnail::thheight = 120;

Thumbnail::Thumbnail(int n): num(n)
{
}

Thumbnail::Thumbnail(int n, const QImage &i): num(n)
{
	setImage(i);
}

Thumbnail::~Thumbnail()
{
}

int Thumbnail::page() const
{
	return num;
}

const QImage& Thumbnail::image() const
{
	return img;
}

void Thumbnail::touch(const QString &fname)
{
	utime(fname.toLocal8Bit(), NULL);
}

bool Thumbnail::tryLoad(const QString &fname)
{
	QImage tmp;
	if (tmp.load(fname))
	{
		setImage(tmp);
		return true;
	}
	return false;
}

bool Thumbnail::save(const QString &fname)
{
	img.save(fname, "JPEG", 75);
}

void Thumbnail::setImage(const QImage &i)
{
	if (i.width() > thwidth || i.height() > thheight)
		img = i.scaled(thwidth, thheight, Qt::KeepAspectRatio);
	else
		img = i.copy();
}

int Thumbnail::maxWidth()
{
	return thwidth;
}

int Thumbnail::maxHeight()
{
	return thheight;
}

