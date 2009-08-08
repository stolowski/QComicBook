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

#ifndef __JUMPTOPAGEWIN_H
#define __JUMPTOPAGEWIN_H

#include <QDialog>

namespace QComicBook
{
	class PageNumberEdit;

	class JumpToPageWindow: public QDialog
	{
		Q_OBJECT

		private:
			PageNumberEdit *pedit;

		protected slots:
			void jumpPressed();

		signals:
			void pageSelected(int n);

		public:
			JumpToPageWindow(QWidget *parent, int value, int max);
			virtual ~JumpToPageWindow();
	};
}

#endif

