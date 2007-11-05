/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <pawel.stolowski@wp.pl>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __SUPARCHWIN_H
#define __SUPARCHWIN_H

/*! \file suparchwin.h */

#include <QDialog>

namespace QComicBook
{
	class SupportedArchivesWindow: public QDialog
	{
		Q_OBJECT

		public:
			SupportedArchivesWindow(QWidget *parent);
			virtual ~SupportedArchivesWindow();
	};
}

#endif

