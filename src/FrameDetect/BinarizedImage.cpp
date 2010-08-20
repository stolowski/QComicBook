/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "BinarizedImage.h"
#include <QImage>

using namespace QComicBook;

BinarizedImage::BinarizedImage(const QImage &img, int thmin)
    : AbstractImage<unsigned char>(img.width(), img.height())
{
    int i = 0;
    for (int y=0; y<img.height(); y++)
    {
       for (int x=0; x<img.width(); x++)
       {
           const QRgb rgb(img.pixel(x, y));
           int g = qGray(rgb);
           if (g>= thmin)
           {
               data[i++] = 255;
           }
           else
           {
               data[i++] = 0;
           }
       }
    }
}

void BinarizedImage::invert()
{
	for (int i=0; i<width()*height(); i++)
	{
		data[i] = 255 - data[i];
	}
}

QImage BinarizedImage::toImage()
{
    QImage img(width(), height(), QImage::Format_RGB32);
    for (int y=0; y<height(); y++)
    {
        for (int x=0; x<width(); x++)
        {
            int p = at(x, y);
            img.setPixel(x, y, qRgb(p, p ,p));
        }
    }
    return img;
}
