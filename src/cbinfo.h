/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005 Pawel Stolowski <yogin@linux.bydg.org>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __CBINFO_H
#define __CBINFO_H

#include <qtabdialog.h>
#include <qfont.h>

class QStringList;
class ImgSink;

class ComicBookInfo: public QTabDialog
{
	protected:
		QFont font;

		void setupGeneralTab(ImgSink &sink);
		void setupDescriptionTabs(const ImgSink &sink);

	public:
		ComicBookInfo(QWidget *parent, ImgSink &sink, const QFont& f);
		virtual ~ComicBookInfo();
};

#endif

