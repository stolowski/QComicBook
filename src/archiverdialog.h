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

#ifndef __ARCHIVERWIN_H
#define __ARCHIVERWIN_H

#include <qdialog.h>

/*! \file archiverwin.h */

class QLineEdit;
class QComboBox;
class QProgressBar;

namespace QComicBook
{
	class ImgDirSink;
	class PagesDirector;

	class ArchiverDialog: public QDialog
	{
		Q_OBJECT

		private:
			QLineEdit *le_destname;
			QComboBox *cm_archtype;
			QProgressBar *pbar;
			PagesDirector *pagesdir;
			ImgDirSink *imgsink;

		public slots:
			void browse();
			void create();

		public:
			ArchiverDialog(QWidget *parent, ImgDirSink *sink);
			virtual ~ArchiverDialog();
	};
}

#endif

