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

#ifndef __FRAMEDETECT_H
#define __FRAMEDETECT_H

#include <QList>
#include <ComicFrameList.h>
#include <AbstractImage.h>
#include <ComicFrame.h>
#include <stdexcept>

class QImage;

namespace QComicBook
{
    class BinarizedImage;
    class ComicFrame;
	class Page;

    typedef AbstractImage<int> LabelData;

    class FrameDetect
    {
	public:
	    FrameDetect(const Page &page);
	    ~FrameDetect();
	    void process();
	    ComicFrameList frames() const;

	    QImage labelToImage();
	    void dump();

	protected:
	    struct Point {
		int x, y;
		Point(int x, int y): x(x), y(y) {}
		bool operator==(const Point &p) const { return x == p.x && y == p.y; }
	    };

		void addWhiteBorders();
		static int determineBackground(const BinarizedImage &img);

		inline static void nextPoint(int &x, int &y, int pos) 
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

		inline void nextPoint(int &offset, int pos)
		{
			switch (pos)
			{
				case 0: ++offset; break;
				case 1: offset += 1 + w; break;
				case 2: offset += w; break;
				case 3: offset += w-1; break;
				case 4: --offset; break;
				case 5: offset -= w+1; break;
				case 6: offset -= w; break;
				case 7: offset += w-1; break;
				default:
						throw std::runtime_error("Invalid position");
			}
		}
	    Point tracer(int x, int y, int &pos, int label);
	    void contourTracking(int x, int y, int initialPos, int label);

	private:
	    BinarizedImage *bimg;
	    LabelData *ldata;
		int page;
	    int ccolor; //contour color
	    int bcolor; //background color
	    int label;
		int w, h;
    };
}

#endif 
