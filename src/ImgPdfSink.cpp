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

#include "ImgPdfSink.h"
#include "Thumbnail.h"
#include "Page.h"
#include <QX11Info>

using namespace QComicBook;

ImgPdfSink::ImgPdfSink(): pdfdoc(0)
{
}

ImgPdfSink::~ImgPdfSink()
{
	delete pdfdoc;
}

int ImgPdfSink::open(const QString &path)
{
	emit progress(0, 1);
	pdfdoc = Poppler::Document::load(path);
	if (!pdfdoc || pdfdoc->isLocked())
	{
		delete pdfdoc;
		pdfdoc = 0;
		return SINKERR_NOTFOUND;
	}
	pdfdoc->setRenderHint(Poppler::Document::Antialiasing);
	emit progress(1, 1);
	return 0;
}

void ImgPdfSink::close()
{	
	delete pdfdoc;
	pdfdoc = 0;
}

Page ImgPdfSink::getImage(unsigned int num, int &result)
{
	if (pdfdoc)
	{
		Poppler::Page* pdfpage = pdfdoc->page(num);
		if (pdfpage)
		{
			QImage img = pdfpage->renderToImage(QX11Info::appDpiX(), QX11Info::appDpiY()); //TODO use defaults if not using X11 (e.g. MS Win)
			delete pdfpage;
			Page p(num, img);
			return p;
		}
	}
	return Page(); //TODO
}

Thumbnail ImgPdfSink::getThumbnail(int num, bool thumbcache)
{
}

int ImgPdfSink::numOfImages() const
{
	return pdfdoc ? pdfdoc->numPages() : -1;
}
