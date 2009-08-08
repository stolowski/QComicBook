/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2007 Pawel Stolowski <pawel.stolowski@wp.pl>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __DIRREADER_H
#define __DIRREADER_H

#include <QDir>
#include <QFileInfo>

class DirReader
{
	private:
		QDir::SortFlags flags;
		int maxDirDepth;
		int curDepth;

		void recurseDir(const QString &path);
		
	protected:
		virtual bool fileHandler(const QFileInfo &path) = 0;

	public:
		DirReader(QDir::SortFlags sortFlags, int maxDepth);
		virtual ~DirReader();

		void visit(const QString &path);
};

#endif

