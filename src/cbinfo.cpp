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

#include "cbinfo.h"
#include <qstringlist.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qfont.h>
#include <imgsink.h>

ComicBookInfo::ComicBookInfo(QWidget *parent, ImgSink &sink, const QFont &f): QTabDialog(parent), font(f)/*{{{*/
{
	setModal(true);
	setCaption("QComicBook info");
	resize(640, 480);

	setupDescriptionTabs(sink);
	setupGeneralTab(sink);
}/*}}}*/

void ComicBookInfo::setupDescriptionTabs(const ImgSink &sink)/*{{{*/
{
	const QStringList desc = sink.getDescription();
	QStringList::const_iterator it = desc.begin();
	while (it != desc.end())
	{
		QWidget *w = new QWidget(this);
		QVBoxLayout *l = new QVBoxLayout(w, 5, 5);
		QTextEdit *txt = new QTextEdit(w);
		l->addWidget(txt);
		addTab(w, *it);
		it++;
		if (it == desc.end())
			break;
		txt->setTextFormat(Qt::PlainText);
		txt->setReadOnly(true);
		txt->setFont(font);
		txt->setText(*it);
		it++;
	}
}/*}}}*/

void ComicBookInfo::setupGeneralTab(ImgSink &sink)/*{{{*/
{
	QWidget *w = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(w, 5, 5);
	QTextEdit *txt = new QTextEdit(w);
	l->addWidget(txt);
	txt->setTextFormat(Qt::PlainText);
	txt->setReadOnly(true);
	txt->setFont(font);
	txt->setText(sink.getStats());

	addTab(w, tr("General info"));
}/*}}}*/

ComicBookInfo::~ComicBookInfo()/*{{{*/
{
}/*}}}*/

