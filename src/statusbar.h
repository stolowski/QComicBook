/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <yogin@linux.bydg.org>
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
class QProgressBar;

namespace QComicBook
{
	class ImlibImage;

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
			void setImageInfo(const ImlibImage *img1, const ImlibImage *img2=NULL);
			void setProgress(int n, int total);
			void clear();

		protected:
			void createInfoElements();
			void removeInfoElements();

		public:
			StatusBar(QWidget *parent, bool elms=true);
			~StatusBar();
	};
}

#endif

