/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __GOTOPAGEWIDGET_H
#define __GOTOPAGEWIDGET_H

#include "ui_GoToPageWidget.h"
#include <QDialog>

namespace QComicBook
{
	class GoToPageWidget: public QDialog, private Ui::GoToPageWidget
	{
		Q_OBJECT

		signals:
			void pageSelected(int n);

		protected slots:
			void goClicked();

		public:
			GoToPageWidget(QWidget *parent, int value, int max);
			virtual ~GoToPageWidget();
	};
}

#endif

