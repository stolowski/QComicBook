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

#include "thumbnail.h"

int Thumbnail::thwidth = 100;
int Thumbnail::thheight = 120;

Thumbnail::Thumbnail(int n, const QImage &i): num(n), img(i)/*{{{*/
{
	img.detach();
	if (i.width() > thwidth || i.height() > thheight)
		qWarning("Thumbnail width/height too large\n");
}/*}}}*/

Thumbnail::~Thumbnail()/*{{{*/
{
}/*}}}*/

int Thumbnail::page() const/*{{{*/
{
	return num;
}/*}}}*/

const QImage& Thumbnail::image() const/*{{{*/
{
	return img;
}/*}}}*/

int Thumbnail::maxWidth()/*{{{*/
{
	return thwidth;
}/*}}}*/

int Thumbnail::maxHeight()/*{{{*/
{
	return thheight;
}/*}}}*/

