/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __PRINT_PROGRESS_DIALOG_H
#define __PRINT_PROGRESS_DIALOG_H

#include <QDialog>
#include "ui_PrintProgressDialog.h"

namespace QComicBook
{
    class PrintProgressDialog: public QDialog, private Ui::PrintProgressDialog
    {
    Q_OBJECT

    public:
        PrintProgressDialog(QWidget *parent);

    public slots:
        void setPage(int n);
    signals:
        void abort();
    };
}

#endif
