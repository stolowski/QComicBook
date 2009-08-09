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
#include <QCryptographicHash>
#include <ComicBookSettings.h>
#include <Utility.h>

using namespace QComicBook;

int Thumbnail::thwidth = 100;
int Thumbnail::thheight = 120;

Thumbnail::Thumbnail(int n, const QString &comicbookName)
  : num(n)
    //  , hash(QCryptographicHash::hash((comicbookName + "." + QString::number(n)).toAscii(), QCryptographicHash::Sha1))    

{
    hash = getScrambledName(comicbookName + "." + QString::number(n));
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

bool Thumbnail::tryLoad()
{
    QImage tmp;
    const QString fname(getFullPath());
    if (tmp.load(fname))
    {
        Utility::touch(fname);
        setImage(tmp);
        return true;
    }
    return false;
}

bool Thumbnail::fromOriginalImage(const QString &fname)
{
    const QImage i(fname);
    if (i.isNull())
        return false;
    setImage(i);
    return true;
}

bool Thumbnail::save()
{
    img.save(getFullPath(), "JPEG", 75);
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

QString Thumbnail::getFullPath() const
{
    return ComicBookSettings::thumbnailsDir() + "/" + hash.toHex() + ".jpg";
}

QByteArray Thumbnail::getScrambledName(const QString &in)
{
    return QCryptographicHash::hash( in.toAscii(), QCryptographicHash::Sha1 );
}
