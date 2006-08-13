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

#include "archiverdialog.h"
#include "imgdirsink.h"
#include "imgarchivesink.h"
#include "pagesdirector.h"
#include <qlayout.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qframe.h>
#include <qcombobox.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qprogressbar.h>
#include <qsizepolicy.h>
#include <qmessagebox.h>

using namespace QComicBook;

ArchiverDialog::ArchiverDialog(QWidget *parent, ImgDirSink *sink): QDialog(parent), imgsink(sink)
{
	setCaption(tr("QComicBook archive creator"));
	resize(560, 480);
			
	QVBoxLayout *lay0 = new QVBoxLayout(this, 5, 5);
	
	QVBox *box0 = new QVBox(this);	
	box0->setMargin(5);
	box0->setSpacing(5);
	box0->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));
	box0->setFrameStyle(QFrame::Panel|QFrame::Sunken);
	lay0->addWidget(box0);

	QHBox *box1 = new QHBox(box0);
	box1->setSpacing(5);
	new QLabel(tr("File name"), box1);
	le_destname = new QLineEdit(box1);
	QPushButton *b_browse = new QPushButton(tr("Browse"), box1);
	connect(b_browse, SIGNAL(clicked()), this, SLOT(browse()));

	QHBox *box2 = new QHBox(box0);
	box2->setSpacing(5);
	new QLabel(tr("Archive type"), box2);
	cm_archtype = new QComboBox(box2);
	cm_archtype->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));
	QValueList<ImgArchiveSink::ArchiveTypeInfo> archlist = ImgArchiveSink::supportedArchivesInfo();
	for (QValueList<ImgArchiveSink::ArchiveTypeInfo>::const_iterator it = archlist.begin(); it != archlist.end(); it++)
	{
		const ImgArchiveSink::ArchiveTypeInfo &inf = *it;
		if (inf.writing)
			cm_archtype->insertItem(inf.name);
	}
	
	QHBoxLayout *lay1 = new QHBoxLayout(lay0);
	pagesdir = new PagesDirector(this, sink);
	lay1->addWidget(pagesdir);
	QVBoxLayout *lay2 = new QVBoxLayout(lay1);
	QPushButton *b_moveup = new QPushButton(tr("Up"), this);
	QPushButton *b_movedown = new QPushButton(tr("Down"), this);
	QPushButton *b_first = new QPushButton(tr("Make first"), this);
	QPushButton *b_last = new QPushButton(tr("Make last"), this);
	connect(b_moveup, SIGNAL(clicked()), pagesdir, SLOT(moveUpSelected()));
	connect(b_movedown, SIGNAL(clicked()), pagesdir, SLOT(moveDownSelected()));
	connect(b_first, SIGNAL(clicked()), pagesdir, SLOT(makeSelectedFirst()));
	connect(b_last, SIGNAL(clicked()), pagesdir, SLOT(makeSelectedLast()));
	lay2->addWidget(b_moveup);
	lay2->addWidget(b_movedown);
	lay2->addWidget(b_first);
	lay2->addWidget(b_last);
	lay2->addStretch();

	QHBoxLayout *lay3 = new QHBoxLayout(NULL, 5, 5);
	b_cancel = new QPushButton(tr("Cancel"), this);
	b_create = new QPushButton(tr("Create"), this);
	connect(b_cancel, SIGNAL(clicked()), this, SLOT(close()));
	connect(b_create, SIGNAL(clicked()), this, SLOT(create()));
	lay3->addStretch();
	lay3->addWidget(b_create);
	lay3->addWidget(b_cancel);
	pbar = new QProgressBar(this);
	pbar->setPercentageVisible(true);
	lay0->addWidget(pbar);
	lay0->addLayout(lay3);
}

ArchiverDialog::~ArchiverDialog()
{
}

void ArchiverDialog::create()
{
	b_create->setDisabled(true);
	b_cancel->setDisabled(true);
	archive = new ImgArchiveSink(*imgsink);
	connect(archive, SIGNAL(createProgress(int, int)), pbar, SLOT(setProgress(int, int)));
	connect(archive, SIGNAL(createReady()), this, SLOT(createReady()));
	connect(archive, SIGNAL(createError(int)), this, SLOT(createError(int)));

	const QString choosen = cm_archtype->currentText();
	QValueList<ImgArchiveSink::ArchiveTypeInfo> archlist = ImgArchiveSink::supportedArchivesInfo();
	for (QValueList<ImgArchiveSink::ArchiveTypeInfo>::const_iterator it = archlist.begin(); it != archlist.end(); it++)
	{
		const ImgArchiveSink::ArchiveTypeInfo &inf = *it;
		if (choosen == inf.name)
		{
			archive->create(le_destname->text(), inf.type, pagesdir->markedPages());
			break;
		}
	}
}

void ArchiverDialog::createReady()
{
	archive->deleteLater();
        QMessageBox::information(this, "QComicBook", tr("Archive successfully created!"), QMessageBox::Ok);
}

void ArchiverDialog::createError(int code)
{
	archive->deleteLater();
        QMessageBox::critical(this, "QComicBook:", tr("Error creating archive!"), QMessageBox::Ok, QMessageBox::NoButton);
}

void ArchiverDialog::browse()
{        
	const QString file = QFileDialog::getSaveFileName(imgsink->getFullName(),
                        "Archives (" + ImgArchiveSink::supportedSaveExtensions() + ");;All files (*)",
                        this, NULL, tr("Choose a file") );
        if (!file.isEmpty() && file != le_destname->text())
	{
		le_destname->setText(file);
		//
		// set combo-box archive type to match extension of choosen file
		QValueList<ImgArchiveSink::ArchiveTypeInfo> archlist = ImgArchiveSink::supportedArchivesInfo();
		for (QValueList<ImgArchiveSink::ArchiveTypeInfo>::const_iterator it = archlist.begin(); it != archlist.end(); it++)
		{
			const ImgArchiveSink::ArchiveTypeInfo &inf = *it;
			if (inf.writing)
				for (QStringList::const_iterator sit = inf.extensions.begin(); sit != inf.extensions.end(); sit++)
				{
					if (file.endsWith(*sit))
					{
						cm_archtype->setCurrentText(inf.name);
						return;
					}
				}
		}
	}
}

