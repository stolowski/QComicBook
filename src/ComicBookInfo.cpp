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

#include "ComicBookInfo.h"
#include "ImgSink.h"
#include <QTabWidget>
#include <QStringList>
#include <QTextEdit>
#include <QLayout>
#include <QFont>
#include <QDialogButtonBox>

using namespace QComicBook;

ComicBookInfo::ComicBookInfo(QWidget *parent, ImgSink &sink, const QFont &f): QDialog(parent), font(f)
{
	setModal(true);
	setWindowTitle(tr("QComicBook info"));
	resize(640, 480);

	QVBoxLayout *lay = new QVBoxLayout(this);
	tabs = new QTabWidget(this);
	lay->addWidget(tabs);

	setupDescriptionTabs(sink);
	//setupGeneralTab(sink);
	
	//
	// setup buttons
	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
	lay->addWidget(buttons);
	connect(buttons, SIGNAL(rejected()), this, SLOT(accept()));
}

void ComicBookInfo::setupDescriptionTabs(const ImgSink &sink)
{
	const QStringList desc = sink.getDescription(); 
	QStringList::const_iterator it = desc.begin();
	while (it != desc.end())
	{
		QWidget *w = new QWidget(this);
		QVBoxLayout *l = new QVBoxLayout(w);
		QTextEdit *txt = new QTextEdit(w);
		l->addWidget(txt);
		tabs->addTab(w, *it);
		it++;
		if (it == desc.end())
			break;
		txt->setReadOnly(true);
		txt->setFont(font);
		txt->setPlainText(*it);
		it++;
	}
}

void ComicBookInfo::setupGeneralTab(ImgSink &sink)
{
	/*QWidget *w = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(w);
	QTextEdit *txt = new QTextEdit(w);
	l->addWidget(txt);
	txt->setReadOnly(true);
	txt->setFont(font);
	//txt->setText(sink.getStats());

	tabs->addTab(w, tr("General info"));*/
}

ComicBookInfo::~ComicBookInfo()
{
}

