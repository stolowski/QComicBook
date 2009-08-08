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

#ifndef __ARCHIVERWIN_H
#define __ARCHIVERWIN_H

#include <QDialog>

/*! \file archiverwin.h */

class QLineEdit;
class QComboBox;
class QProgressBar;
class QPushButton;

namespace QComicBook
{
	class ImgDirSink;
	class ImgArchiver;
	class PagesDirector;

	class ArchiverDialog: public QDialog
	{
		Q_OBJECT

		private:
			QLineEdit *le_destname;
			QComboBox *cm_archtype;
			QProgressBar *pbar;
			QPushButton *b_cancel;
			QPushButton *b_create;
			PagesDirector *pagesdir;
			ImgDirSink *imgsink; //original sink (directory or archive)
			ImgArchiver *archive; //new archive

		protected slots:
			void browse();
			void create();
			void createReady();
			void createError(int code);

		public:
			ArchiverDialog(QWidget *parent, ImgDirSink *sink);
			virtual ~ArchiverDialog();
	};
}

#endif

