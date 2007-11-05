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

#ifndef __HELPBROWSER_H
#define __HELPBROWSER_H

#include <qmainwindow.h>

class QTextBrowser;
class QPopupMenu;

namespace QComicBook
{
	class HelpBrowser: public QMainWindow
	{
		Q_OBJECT

		private:
			QTextBrowser *txtb;
			QPopupMenu *go_menu;
			int id_back;
			int id_forward;

		private slots:
			void enableBackward(bool f);
			void enableForward(bool f);

		public:
			HelpBrowser(const QString &caption, const QString &path, const QString &file="index.html", const QString &imgpath="img", QWidget *parent=NULL);
			virtual ~HelpBrowser();
			static QString getLocaleHelpDir(const QString &maindir, const QString &file=QString::null);
	};
}

#endif

