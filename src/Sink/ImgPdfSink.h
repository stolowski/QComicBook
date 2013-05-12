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

			int open(const QString &path) override;
                        virtual void sort(const PageSorter &sorter) override;
			void close() override;
			QImage image(unsigned int num, int &result) override;
			int numOfImages() const override;
			QStringList getDescription() const { return QStringList(); }
			bool supportsNext() const override { return false; }
			QString getNext() const override { return ""; }
			QString getPrevious() const override { return ""; }

		private:
			Poppler::Document *pdfdoc;
			mutable QMutex docmtx; //!< mutex for pdf document
	};
}

#endif
