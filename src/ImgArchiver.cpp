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

#include "ImgArchiver.h"
#include <QProcess>
#include <QApplication>
#include <QDir>

using namespace QComicBook;

ImgArchiver::ImgArchiver(const ImgDirSink &sink): ImgArchiveSink(sink)
{
	pomp = new QProcess(this);
	connect(pomp, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(compressExited(int, QProcess:ExitStatus)));
	connect(pomp, SIGNAL(readyReadStdandardOutput()), this, SLOT(compressStdoutReady()));
}

ImgArchiver::~ImgArchiver()
{
	doCleanup();
}

void ImgArchiver::compressStdoutReady()
{
	QByteArray b = pomp->readAllStandardOutput();
	for (int i=0; i<b.size(); i++)
		if (b[i] == '\n')
			++cmpcnt;
	emit createProgress(cmpcnt, newfilesnum);
	qApp->processEvents();
}

void ImgArchiver::compressExited(int exitCode, QProcess::ExitStatus status)
{
	doCleanup();
	if (status == QProcess::NormalExit)
		emit createReady();
	else
		emit createError(0);
}

void ImgArchiver::doCleanup()
{
	if (!tmpcreatpath.isEmpty())
	{
		QDir dir(tmpcreatpath);
		//
		// remove temporary files and dirs
		for (QStringList::const_iterator it = newfiles.begin(); it !=newfiles.end(); ++it)
			dir.remove(*it);
		dir.rmdir(tmpcreatpath);
		tmpcreatpath = QString::null;
	}
}

void ImgArchiver::create(const QString &destname, ArchiveType type, QList<int> pages)
{
	bool status = false;
	QStringList args;

	//
	// match archive type, set subprocess options
	for (QList<ArchiveTypeInfo>::const_iterator it = archinfo.begin(); it!=archinfo.end(); it++)
	{
		const ArchiveTypeInfo &inf = *it;
		if (type == inf.type)
		{
			args = inf.compressopts;
			status = true;
			break;
		}
	}

	if (!status)
	{
		emit createError(SINKERR_NOTSUPPORTED);
		return;
	}

	tmpcreatpath = makeTempDir();
	args << destname;
	args << "*";

	char *fmt;
	if (pages.size() < 100)
		fmt = "%.2d";
	else
		fmt = "%.3d";

	int cnt = 0; //pages counter, used to create file names with new numbering scheme
	for (QList<int>::const_iterator it = pages.begin(); it != pages.end(); it++)
	{
		QString fname;
		QString ext = getKnownImageExtension(getFullFileName(*it));
		if (ext.isEmpty())
			; //TODO
		fname.sprintf(fmt, cnt++);
		fname = tmpcreatpath + "/" + fname + ext;
		if (symlink(getFullFileName(*it).toLocal8Bit(), fname.toLocal8Bit()) == 0)
			newfiles.append(fname);
		else
			; //TODO: error handling

	}

	pomp->setWorkingDirectory(tmpcreatpath);

	cmpcnt = 0;
	newfilesnum = pages.count();
	QString prg = args.takeFirst();
	//if (!pomp->start(prg, args)) //TODO
	//	emit createError(0);
}

