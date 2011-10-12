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
#include <CompareFrames.h>
#include "../ComicBookDebug.h"

using namespace QComicBook;

ComicFrameList::ComicFrameList(int page, int width, int height)
	: m_page(page)
	  , m_pageWidth(width)
	  , m_pageHeight(height)
{
}

ComicFrameList::~ComicFrameList()
{
}

void ComicFrameList::append(const ComicFrame &f)
{
	m_frames.append(f);
}

void ComicFrameList::sort(bool manga)
{
    _DEBUG;
    std::sort(m_frames.begin(), m_frames.end(), CompareFrames(static_cast<double>(m_pageHeight) * 0.05f, manga));
}

const ComicFrame& ComicFrameList::operator[](int idx)
{
	return m_frames[idx];
}

