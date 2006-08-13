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

#include <qpushbutton.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include "jumptopagewin.h"
#include "pagenumberedit.h"

using namespace QComicBook;

JumpToPageWindow::JumpToPageWindow(QWidget *parent, int value, int max): QDialog(parent)
{
	setCaption(tr("Jump to page..."));
	setModal(true);
	
	QHBoxLayout *lay = new QHBoxLayout(this, 5, 5);
	
	QHBox *box = new QHBox(this);
	lay->addWidget(box);
	box->setSpacing(5);
	new QLabel(tr("Page number"), box);
	pedit = new PageNumberEdit(box, value, max);
	new QLabel(QString("/") + QString::number(max), box);
	QPushButton *b_ok = new QPushButton(tr("Jump"), box);
	connect(b_ok, SIGNAL(clicked()), this, SLOT(jumpPressed()));
	connect(b_ok, SIGNAL(clicked()), this, SLOT(close()));
	b_ok->setDefault(true);
	connect(pedit, SIGNAL(pageSelected(int)), this, SIGNAL(pageSelected(int)));
	pedit->setFocusPolicy(QWidget::StrongFocus);
	pedit->setFocus();
	pedit->deselect();
	setFixedWidth(280);
}

JumpToPageWindow::~JumpToPageWindow()
{
}

void JumpToPageWindow::jumpPressed()
{
	int n = pedit->pageNumber();
	setResult(n);
	emit pageSelected(n);
}

