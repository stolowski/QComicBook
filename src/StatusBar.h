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

#ifndef __STATUSBAR_H
#define __STATUSBAR_H

#include <QStatusBar>

class QLabel;
class QProgressBar;
class QImage;

namespace QComicBook
{
	class StatusBar: public QStatusBar
	{
		Q_OBJECT

		private:
			QLabel *page;
			QLabel *imginfo;
			QLabel *name;
			QProgressBar *pbar;

		public slots:
			void setPage(int n, int total);
			void setName(const QString &n);
			void setImageInfo(const QImage *img1, const QImage *img2=NULL);
			void setProgress(int n, int total);
			void clear();

		protected:
			void createInfoElements();
			void removeInfoElements();
			void createProgressBar();
			void removeProgressBar();

		public:
			StatusBar(QWidget *parent, bool elms=true);
			virtual ~StatusBar();
	};
}

#endif

