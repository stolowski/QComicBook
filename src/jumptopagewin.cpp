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

#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include "jumptopagewin.h"
#include "pagenumberedit.h"

using namespace QComicBook;

JumpToPageWindow::JumpToPageWindow(QWidget *parent, int value, int max): QDialog(parent)
{
	setWindowTitle(tr("Jump to page..."));
	setModal(true);
	
	QHBoxLayout *lay = new QHBoxLayout(this);
	
	lay->addWidget(new QLabel(tr("Page number"), this));
	pedit = new PageNumberEdit(this, value, max);
	lay->addWidget(pedit);
	lay->addWidget(new QLabel(QString("/") + QString::number(max), this));
	QPushButton *b_ok = new QPushButton(tr("Jump"), this);
	lay->addWidget(b_ok);
	connect(b_ok, SIGNAL(clicked()), this, SLOT(jumpPressed()));
	connect(b_ok, SIGNAL(clicked()), this, SLOT(close()));
	b_ok->setDefault(true);
	connect(pedit, SIGNAL(pageSelected(int)), this, SIGNAL(pageSelected(int)));
	pedit->setFocus();
	pedit->setFocusPolicy(Qt::StrongFocus);
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

