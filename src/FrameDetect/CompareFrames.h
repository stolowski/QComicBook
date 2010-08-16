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

#ifndef __COMPARE_FRAMES_H
#define __COMPARE_FRAMES_H

#include <ComicFrame.h>

namespace QComicBook
{

	//! Simple Comic Frame comparer.
	/*! Sorts frames left-right or right-left, top-bottom based on X & Y coordinates. 
	 *  Supports tolerance when comparing Y coordinates.
	 */
	class CompareFrames
	{
		public:
			CompareFrames(int ytolerance): m_ytolerance(ytolerance) {}

			static bool between(int x, int a, int b)
			{
				return x > a && x < b;
			}

			static int sharedHeight(int y1, int h1, int y2, int h2)
			{
				return std::min(y1 + h1, y2 + h2) - std::max(y1, y2);
			}

			bool operator()(const ComicFrame &f1, const ComicFrame &f2)
			{
				if (std::abs(f1.yPos() - f2.yPos()) <= m_ytolerance || std::abs(f1.yPos() + f1.height() - f2.yPos() - f2.height() <= m_ytolerance))
				{
					return f1.xPos() < f2.xPos();
				}
				else
				{
					if (f1.xPos() < f2.xPos() && between(f1.yPos(), f2.yPos(), f2.yPos() + f2.height()))
					{
						if (static_cast<double>(sharedHeight(f1.yPos(), f1.height(), f2.yPos(), f2.height())) / static_cast<double>(f1.height()) > 0.5f)
						{
							return false;
						}
					}
				}
				return f1.yPos() < f2.yPos();
			}

		private:
			const int m_ytolerance;
	};
}

#endif
