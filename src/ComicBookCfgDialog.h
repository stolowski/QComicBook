/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <pawel.stolowski@wp.pl>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __CBCONFIGDIALOG_H
#define __CBCONFIGDIALOG_H

#include <QDialog>
#include "ui_ConfigDialog.h"

namespace QComicBook
{
	class ComicBookSettings;
	class ComicBookCfgDialog: public QDialog, private Ui::ConfigDialog
	{
		Q_OBJECT

		private:
			ComicBookSettings *cfg;
			QColor bgcolor;
			QFont font;

			void updateFontPreview();

		protected slots:
			virtual void accept();
			void showBackgroundDialog();
			void showFontDialog();

		public:
			ComicBookCfgDialog(QWidget *parent, ComicBookSettings *cfg);
			virtual ~ComicBookCfgDialog();
	};
}

#endif

