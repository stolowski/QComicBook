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

#ifndef __ABOUT_DIALOG_H
#define __ABOUT_DIALOG_H

#include <QDialog>
#include "ui_AboutDialog.h"

namespace QComicBook
{
    class AboutDialog: public QDialog, private Ui::AboutDialog
    {
        public:
		AboutDialog(QWidget *parent, const QString &caption, const QString &text, const QPixmap &logo);
		virtual ~AboutDialog();
		void setText(const QString &text);
    };
}
#endif

