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

#ifndef __PAGENUMBEREDIT_H
#define __PAGENUMBEREDIT_H

#include <qlineedit.h>

class QIntValidator;

namespace QComicBook
{
	class PageNumberEdit: public QLineEdit
	{
		Q_OBJECT

		private:
			QIntValidator *validator;

			protected slots:
				void onReturn();
			void selectAll();

		signals:
			void pageSelected(int n);

		public:
			PageNumberEdit(QWidget *parent, int val=1, int max=99);
			virtual ~PageNumberEdit();
			void setMax(int n);
			int pageNumber() const;
	};
}

#endif

