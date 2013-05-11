/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include <algorithm>
#include "DirReader.h"
#include "NaturalComparator.h"

DirReader::DirReader(QDir::SortFlags sortFlags, int maxDepth): flags(sortFlags), maxDirDepth(maxDepth)
{
}

DirReader::~DirReader()
{
}

void DirReader::recurseDir(const QString &path, int curDepth)
{
	QDir dir(path);
	dir.setSorting(flags);
	dir.setFilter(QDir::AllDirs|QDir::Files);
	QStringList files = dir.entryList();
	std::sort(files.begin(), files.end(), NaturalComparator());

	foreach (QString f, files)
        {
            if (f == "." || f == "..") //FIXME: use no-dot and no-dot-dot filter
                continue;
            QFileInfo finf(dir, f);
            fileHandler(finf);
            if (finf.isDir()) // && (finf.absoluteFilePath() != path))
            {
                if (curDepth < maxDirDepth)
                {
                    recurseDir(finf.absoluteFilePath(), curDepth+1);
                }
            }
	}
}

void DirReader::visit(const QString &path)
{
	recurseDir(path, 0);
}
