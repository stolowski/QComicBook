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

#include "ImgPdfSink.h"
#include "../Page.h"
#include <QFileInfo>
#include <QMutexLocker>
#include <QScreen>
#include <QWidget>

using namespace QComicBook;

ImgPdfSink::ImgPdfSink(QWidget *parent, int cacheSize): ImgSink(cacheSize), parent_(parent)
{
}

ImgPdfSink::~ImgPdfSink()
{
}

int ImgPdfSink::open(const QString &path)
{
	emit progress(0, 1);
	pdfdoc = Poppler::Document::load(path);
	if (!pdfdoc || pdfdoc->isLocked())
	{
		pdfdoc.reset();
		return SINKERR_NOTFOUND;
	}

	pdfdoc->setRenderHint(Poppler::Document::Antialiasing, true);
	pdfdoc->setRenderHint(Poppler::Document::TextAntialiasing, true);
	
	QFileInfo info(path);
	setComicBookName(info.fileName(), path);

	emit progress(1, 1);
	return 0;
}

void ImgPdfSink::close()
{	
	pdfdoc.reset();
}

QImage ImgPdfSink::image(unsigned int num, int &result)
{
	result = 1;
	QMutexLocker lock(&docmtx);
	if (pdfdoc)
	{
		std::unique_ptr<Poppler::Page> pdfpage = pdfdoc->page(num);
		if (pdfpage)
		{
            QScreen *screen = parent_->screen();
			QImage img = pdfpage->renderToImage(screen->logicalDotsPerInchX(), screen->logicalDotsPerInchY());
			result = 0;
			return img;
		}
	}
	return QImage();
}

int ImgPdfSink::numOfImages() const
{
	QMutexLocker lock(&docmtx);
	const int n = pdfdoc ? pdfdoc->numPages() : -1;
	return n;
}
