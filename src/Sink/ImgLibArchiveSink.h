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

				virtual int open(const QString &path) override;
                                virtual void sort(const PageSorter &sorter) override;
				virtual void close() override;
				virtual QImage image(unsigned int num, int &result);
				virtual int numOfImages() const override;
				virtual QStringList getDescription() const override;
				virtual bool supportsNext() const override;
				virtual QString getNext() const override;
				virtual QString getPrevious() const override;
			private:
				QString archivepath;
                                QList<FileEntry> imgfiles;
		};
}

#endif
