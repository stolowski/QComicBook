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

#ifndef __STATUSBAR_H
#define __STATUSBAR_H

#include <qstatusbar.h>

class QLabel;

class StatusBar: public QStatusBar
{
	Q_OBJECT
	
	private:
		QLabel *page;
		QLabel *name;

	public slots:
		void setPage(int n, int total);
		void setName(const QString &n);
		void clear();

	public:
		StatusBar(QWidget *parent);
		~StatusBar();
};

#endif

