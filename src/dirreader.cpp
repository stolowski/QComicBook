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

#include "dirreader.h"

DirReader::DirReader(QDir::SortFlags sortFlags, int maxDepth): flags(sortFlags), maxDirDepth(maxDepth), curDepth(0)
{
}

DirReader::~DirReader()
{
}

void DirReader::recurseDir(const QString &path)
{
        QDir dir(path);
        dir.setSorting(flags);
        dir.setFilter(QDir::AllDirs|QDir::Files);

        const QStringList files = dir.entryList();
	foreach (QString f, files)
        {
                if (f == "." || f == "..")
                        continue;

                QFileInfo finf(dir, f);
		fileHandler(finf);
                if (finf.isDir()) // && (finf.absoluteFilePath() != path))
		{
			if (curDepth < maxDirDepth)
			{
				++curDepth;
				recurseDir(finf.absoluteFilePath());
			}
		}
        }
}

void DirReader::visit(const QString &path) 
{
	recurseDir(path);
}

