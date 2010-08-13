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

#include <FrameDetect.h>
#include <ComicFrame.h>
#include <BinarizedImage.h>
#include <QImage>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <QDebug>

using namespace QComicBook;

FrameDetect::FrameDetect(const QImage &img)
{
    bimg = new BinarizedImage(img, 250);
    bimg->invert();
    bimg->toImage().save("binimg.jpg");
    w = bimg->width();
    h = bimg->height();

    ldata = new LabelData(img.width(), img.height());
    ldata->fill(0);

    ccolor = 255;
    bcolor = 0;
}

FrameDetect::~FrameDetect()
{
    delete bimg;
    delete ldata;
}

void FrameDetect::process()
{
	label = 1;
	for (int y=1; y<bimg->height()-1; y++)
	{
		for (int x=1; x<bimg->width(); x++)
		{
			int &p = ldata->at(x, y);

			if (bimg->at(x, y) == ccolor) // find contour pixel
			{
				//
				// step 1
				if (p == 0 && bimg->at(x, y-1) == bcolor)
				{
					p = label;
					contourTracking(x, y, 7, label++);
				}
				//
				// step 2
				else if (ldata->at(x, y+1) == 0 && bimg->at(x, y+1) == bcolor) //if pixel below is unmarked white pixel
				{
					if (p == 0) //P is unlabeled
					{
						p = ldata->at(x-1, y); //copy label
					}
					contourTracking(x, y, 3, p);
				}
				else if (p == 0)
				{
					p = ldata->at(x-1, y);
				}
			}
		}
	}
	qDebug() << label-1 << "labels";
}

void FrameDetect::nextPoint(int &x, int &y, int pos)
{
	switch (pos)
	{
		case 0: ++x; break;
		case 1: ++x; ++y; break;
		case 2: ++y; break;
		case 3: ++y; --x; break;
		case 4: --x; break;
		case 5: --x; --y; break;
		case 6: --y; break;
		case 7: ++x; --y; break;
		default:
			throw std::runtime_error("Invalid position");
	}
}

FrameDetect::Point FrameDetect::tracer(int x, int y, int &pos, int lbl)
{
	for (int i=0; i<8; i++)
	{
		int tx(x);
		int ty(y);
		nextPoint(tx, ty, pos);
		if (tx >0 && ty >0 && tx < bimg->width() && ty < bimg->height())
		{
			const int l( ldata->at(tx, ty) );
			if (bimg->at(tx, ty) == ccolor && (l == 0 || l == lbl))
			{
				return Point(tx, ty);
			}
			if (bimg->at(tx, ty) == bcolor)
			{
				ldata->at(tx, ty) = -1;
			}
		}
		pos = (pos + 1)%8;
	}
	return Point(-1, -1);
}
    
void FrameDetect::contourTracking(int x, int y, int initialPos, int lbl)
{
	int d(initialPos);
	const Point s(x, y);
	bool startReached(false);
	bool tFollowsS(false);

	Point t = tracer(x, y, d, lbl);
	if (t.x < 0) // isolated point
	{
		return;
	}

	ldata->at(t.x, t.y) = lbl;

	x = t.x;
	y = t.y;

	while (startReached == false || tFollowsS == false)
	{
		d = (d+4)%8;
		d = (d + 2)%8;

		Point f = tracer(x, y, d, lbl);
		if (f.x < 0)
		{
			qDebug() << "img boundary";
			return;
		}

		if (f == s)
		{
			startReached = true;
		}
		else if (f == t)
		{
			if (startReached)
			{
				tFollowsS = true;
			}
		}
		else
		{
			startReached = tFollowsS = false;
		}

		ldata->at(f.x, f.y) = lbl;

		x = f.x;
		y = f.y;

	}
}

QImage FrameDetect::labelToImage()
{
	const int m(std::max(1, 255/label));
	QImage img(ldata->width(), ldata->height(), QImage::Format_RGB32);
	for (int y=0; y<ldata->height(); y++)
	{
		for (int x=0; x<ldata->width(); x++)
		{
			const int l = m * ldata->at(x, y);
			if (l >= 0)
			{
				img.setPixel(x, y, qRgb(l, l, l));
			}
		}
	}
	return img;
}

void FrameDetect::dump()
{
	std::ofstream str("dump.txt");
	for (int y=0; y<ldata->height(); y++)
	{
		for (int x=0; x<ldata->width(); x++)
		{
			str << ldata->at(x, y);
		}
		str << std::endl;
	}
	str.close();
}

QList<ComicFrame> FrameDetect::frames() const
{
	int *x1 = new int [label];
	int *x2 = new int [label];
	int *y1 = new int [label];
	int *y2 = new int [label];

	//
	// initialize lookups
	for (int lbl=0; lbl<label; lbl++)
	{
			x1[lbl] = ldata->width();
			x2[lbl] = y2[lbl] = -1;
			y1[lbl] = ldata->height();
	}

	for (int y=0; y<ldata->height(); y++)
	{
		for (int x=0; x<ldata->width(); x++)
		{
			const int lbl(ldata->at(x, y));
			if (lbl > 0)
			{
				if (x < x1[lbl]) { x1[lbl] = x; }
				if (x > x2[lbl]) { x2[lbl] = x; }
				if (y < y1[lbl]) { y1[lbl] = y; }
				if (y > y2[lbl]) { y2[lbl] = y; }
			}
		}
	}
	
	QList<ComicFrame> frms;
	for (int lbl=0; lbl<label; lbl++)
	{
		const int w(x2[lbl]-x1[lbl]);
		const int h(y2[lbl]-y1[lbl]);
		if ((w >= ldata->width()/6) && (h >= ldata->height()/8))
		{
			frms.append(ComicFrame(x1[lbl], y1[lbl], w, h, lbl));
		}
	}

	delete [] x1;
	delete [] x2;
	delete [] y1;
	delete [] y2;

	return frms;
}
