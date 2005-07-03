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

#include "waitdialog.h"
#include <qlayout.h>
#include <qlabel.h>
#include <qcursor.h>

WaitDialog::WaitDialog(QWidget *parent, const QString &caption, const QString &text): QDialog(parent)
{
	setModal(true);	
	setCaption(caption);
	setCursor(Qt::BusyCursor);
	
	QHBoxLayout *box1 = new QHBoxLayout(this, 5, 5);
	info = new QLabel(text, this);
	box1->addWidget(info);
}

WaitDialog::~WaitDialog()
{
}

void WaitDialog::setText(const QString &text)
{
	info->setText(text);
}
	
