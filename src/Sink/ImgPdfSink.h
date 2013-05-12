/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2011 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __IMGPDFSINK_H
#define __IMGPDFSINK_H

#include "ImgSink.h"
#include <QStringList>
#include <QMutex>
#include <poppler-qt4.h>

namespace QComicBook
{
	class ImgPdfSink: public ImgSink
	{
		public:
			ImgPdfSink(int cacheSize=0);
			~ImgPdfSink();

			int open(const QString &path);
                        virtual void sort(const PageSorter &sorter);
			void close();
			QImage image(unsigned int num, int &result);
			int numOfImages() const;
			QString getName(int maxlen = 50) { return ""; }
			QString getFullName() const { return ""; }
			QString getFullFileName(int page) const { return ""; }
			QStringList getDescription() const { return QStringList(); }
			bool timestampDiffers(int page) const { return false; }
			bool hasModifiedFiles() const { return false; }
			bool supportsNext() const { return false; }
			QString getNext() const { return ""; }
			QString getPrevious() const { return ""; }

		private:
			Poppler::Document *pdfdoc;
			mutable QMutex docmtx; //!< mutex for pdf document
	};
}

#endif
