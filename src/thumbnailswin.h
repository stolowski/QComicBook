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

#ifndef __THUMBNAILSWIN_H
#define __THUMBNAILSWIN_H

#include <QDockWidget>
#include <QEvent>

namespace QComicBook
{
	class ThumbnailsView;

	class ThumbnailsWindow: public QDockWidget
	{
		Q_OBJECT

		private:
			ThumbnailsView *tview;

		signals:
			void requestedThumbnail(int n);
			void requestedPage(int n, bool force);

		protected:
			void customEvent(QEvent *e);

		protected slots:
			//void onOrientationChanged(Orientation o); -- nie ma w Qt4

		public:
			ThumbnailsWindow(QWidget *parent=0);
			virtual ~ThumbnailsWindow();
			ThumbnailsView* view() const;
	};
}

#endif

