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

#ifndef __WAIT_DIALOG_H
#define __WAIT_DIALOG_H

#include <qdialog.h>
#include <qstring.h>

class QLabel;

namespace QComicBook
{
	class WaitDialog: public QDialog
	{
		Q_OBJECT

		private:
			QLabel *info;

		public:
			WaitDialog(QWidget *parent, const QString &caption, const QString &text=QString::null);
			~WaitDialog();
			void setText(const QString &text);
	};
}

#endif

