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

#include "aboutdialog.h"
#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>

AboutDialog::AboutDialog(QWidget *parent, const QString &caption, const QString &text): QDialog(parent)
{
	setCaption(caption);
	setModal(true);
	setMinimumSize(320, 200);

	QVBoxLayout *box1 = new QVBoxLayout(this, 5, 5);
	box1->setResizeMode(QLayout::FreeResize);
	QLabel *about = new QLabel(text, this);
	about->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	about->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
	about->setMargin(5);
	box1->addWidget(about);

	QPushButton *bclose = new QPushButton(tr("Close"), this);
	bclose->setDefault(true);
	bclose->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	box1->addWidget(bclose, 0, Qt::AlignHCenter);
	connect(bclose, SIGNAL(clicked()), this, SLOT(accept()));
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::setText(const QString &text)
{
	about->setText(text);
}

