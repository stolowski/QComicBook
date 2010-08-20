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

#include <FrameCache.h>

using namespace QComicBook;

FrameCache& FrameCache::instance()
{
	static FrameCache cache;
	return cache;
}

FrameCache::FrameCache()
{
}

FrameCache::~FrameCache()
{
}

void FrameCache::insert(const ComicFrameList &frames)
{
	m_frames[frames.pageNumber()] = frames;
}

bool FrameCache::has(int page) const
{
	return m_frames.contains(page);
}

ComicFrameList FrameCache::get(int page) const
{
	return m_frames[page];
}

void FrameCache::clear()
{
	m_frames.clear();
}

