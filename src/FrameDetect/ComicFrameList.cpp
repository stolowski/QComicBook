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

#include <ComicFrameList.h>
#include <algorithm>
#include <QDebug>

using namespace QComicBook;

ComicFrameList::ComicFrameList(int page)
	: m_page(page)
{

}

ComicFrameList::~ComicFrameList()
{

}

struct CompareFrames
{
	bool operator()(const ComicFrame &f1, const ComicFrame &f2)
	{
		const int m = 10;

		if (std::abs(f1.yPos() - f2.yPos()) < m)
		{
			return f1.xPos() < f2.xPos();
		}
		else
		{
			return f1.yPos() < f2.yPos();
		}
	}
};

void ComicFrameList::append(const ComicFrame &f)
{
	m_frames.append(f);
}

void ComicFrameList::sort()
{
	qDebug() << "frames sort";
	std::sort(m_frames.begin(), m_frames.end(), CompareFrames());
}

const ComicFrame& ComicFrameList::operator[](int idx)
{
	return m_frames[idx];
}

