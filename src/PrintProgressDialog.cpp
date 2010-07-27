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

#include "PrintProgressDialog.h"

using namespace QComicBook;

PrintProgressDialog::PrintProgressDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(bcancel, SIGNAL(clicked()), this, SIGNAL(abort()));
}

void PrintProgressDialog::setPage(int n)
{
    page->setText(QString::number(n));
}
