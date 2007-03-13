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

#ifndef __ABOUT_DIALOG_H
#define __ABOUT_DIALOG_H

#include <QDialog>

class QLabel;
class QPixmap;

namespace QComicBook
{
    class AboutDialog: public QDialog
    {
        private:
		QLabel *about_text;
		QLabel *about_logo;

        public:
		AboutDialog(QWidget *parent, const QString &caption, const QString &text, const QPixmap &logo);
		virtual ~AboutDialog();
		void setText(const QString &text);
    };
}
#endif

