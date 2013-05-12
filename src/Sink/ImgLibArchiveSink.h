/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2013 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __IMGLIBARCHIVESINK_H
#define __IMGLIBARCHIVESINK_H

#include "ImgSink.h"
#include <archive.h>

namespace QComicBook
{
    class FileEntry;

	//! Comic book archive sink based on libarchive.
	/*! Allows opening different kind of archives containing image files, without decompressing all files to disk. */
	class ImgLibArchiveSink: public ImgSink
	{
			public:
				ImgLibArchiveSink(int cacheSize=0);

				virtual ~ImgLibArchiveSink();

				virtual void setCacheSize(int cacheSize, bool autoAdjust);

				virtual int open(const QString &path);
                                virtual void sort(const PageSorter &sorter);

				virtual void close();

				virtual QImage image(unsigned int num, int &result);

				virtual Thumbnail getThumbnail(unsigned int num, bool thumbcache=true);

				virtual int numOfImages() const;
				
				void setComicBookName(const QString &name, const QString &fullName);
				QString getName(int maxlen = 50) const;

				QString getFullName() const;

				virtual QString getFullFileName(int page) const;

				virtual QStringList getDescription() const;

				virtual bool supportsNext() const;

				virtual QString getNext() const;

				virtual QString getPrevious() const;
			private:
				QString archivepath;
                                QList<FileEntry> imgfiles;
		};
}

#endif
