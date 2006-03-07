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

#include "imgarchivesink.h"
#include "miscutil.h"
#include <qimage.h>
#include <qstringlist.h>
#include <qprocess.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qapplication.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>

using namespace QComicBook;
using Utility::which;

QString ImgArchiveSink::openext;
QString ImgArchiveSink::saveext;
int ImgArchiveSink::suppopen;
int ImgArchiveSink::suppsave;

QValueList<ImgArchiveSink::ArchiveTypeInfo> ImgArchiveSink::archinfo;

ImgArchiveSink::ImgArchiveSink(int cachesize): ImgDirSink(cachesize), docleanup(true)
{
	init();
}

ImgArchiveSink::ImgArchiveSink(const QString &path, int cachesize): ImgDirSink(cachesize), docleanup(true)
{
	init();
	open(path);
}

ImgArchiveSink::ImgArchiveSink(const ImgDirSink &sink): ImgDirSink(sink), docleanup(false)
{
}

ImgArchiveSink::~ImgArchiveSink()
{
	ImgArchiveSink::close();
}

void ImgArchiveSink::init()
{
	pinf = new QProcess(this);
	pext = new QProcess(this);
	connect(pinf, SIGNAL(readyReadStdout()), this, SLOT(infoStdoutReady()));
	connect(pinf, SIGNAL(processExited()), this, SLOT(infoExited()));
	connect(pext, SIGNAL(readyReadStdout()), this, SLOT(extractStdoutReady()));
	connect(pext, SIGNAL(processExited()), this, SLOT(extractExited()));
}

ImgArchiveSink::ArchiveType ImgArchiveSink::archiveType(const QString &filename)
{
	static const struct {
		int offset; //the first byte to compare
		int len; //number of bytes in pattern
		const char *ptrn; //pattern
		ArchiveType type; //archive type
	} magic[] = {
		{0, 4, "\x52\x61\x72\x21", RAR_ARCHIVE},
		{0, 4, "\x50\x4b\x03\x04", ZIP_ARCHIVE},
		{8, 4, "\x2a\x41\x43\x45", ACE_ARCHIVE}
	};

	QFile file(filename);
	if (file.open(IO_ReadOnly))
	{
		for (int i=0; i<3; i++)
		{
			int j;
			if (!file.at(magic[i].offset))
				continue;
			for (j=0; j<magic[i].len; j++)
			{
				int c;
				if ((c = file.getch()) < 0)
					break;
				if (c != magic[i].ptrn[j])
					break;
			}
			if (j == magic[i].len)
			{
				file.close();
				return magic[i].type;
			}
		}
		file.close();
	}

	//
	// try to match filename extension
	for (QValueList<ArchiveTypeInfo>::const_iterator it = archinfo.begin(); it!=archinfo.end(); it++)
	{
		const ArchiveTypeInfo &inf = *it;
		for (QStringList::const_iterator sit = inf.extensions.begin(); sit!=inf.extensions.end(); sit++)
		{
			if (filename.endsWith(*sit, false))
				return inf.type;
		}
	}

	return UNKNOWN_ARCHIVE;
}

int ImgArchiveSink::extract(const QString &filename, const QString &destdir)
{
	archivetype = archiveType(filename);
	filesnum = 0;

	if (archivetype == UNKNOWN_ARCHIVE)
		return SINKERR_UNKNOWNFILE;
	if (!supportsOpen(archivetype))
		return SINKERR_NOTSUPPORTED;	

	//
	// match archive type, set subprocess extract and list options
	for (QValueList<ArchiveTypeInfo>::const_iterator it = archinfo.begin(); it!=archinfo.end(); it++)
	{
		const ArchiveTypeInfo &inf = *it;
		if (archivetype == inf.type)
		{
			pext->setArguments(inf.extractopts);
			pinf->setArguments(inf.listopts);
			break;
		}
	}
	
	pext->addArgument(filename);
	pinf->addArgument(filename);
	pext->setWorkingDirectory(destdir);

	//
	// extract archive file list first
	if (!pinf->start())
		return SINKERR_OTHER;
	return 0;
}

int ImgArchiveSink::open(const QString &path)
{
	QFileInfo info(path);
	archivepath = path;
	archivename = info.fileName();
	if (!info.exists())
	{
		emit sinkError(SINKERR_NOTFOUND);
		return SINKERR_NOTFOUND;
	}
	if (info.isFile())
	{
		if (info.isReadable())
		{
			char tmpd[19];
			strcpy(tmpd, "/tmp/qcomic-XXXXXX");
			mkdtemp(tmpd);
			int status = extract(path, tmppath = tmpd);
			if (status != 0)
			{
				emit sinkError(status);
				close();
			}
			return status;
		}
		else
		{
			emit sinkError(SINKERR_ACCESS);
			return SINKERR_ACCESS;
		}
	}
	emit sinkError(SINKERR_NOTFILE);
	return SINKERR_NOTFILE;
}

void ImgArchiveSink::close()
{
	ImgDirSink::close();
	QDir dir(tmppath);
	//
	// remove temporary files and dirs
	for (QStringList::const_iterator it = archfiles.begin(); it != archfiles.end(); ++it)
		dir.remove(*it);
	for (QStringList::const_iterator it = archdirs.begin(); it != archdirs.end(); ++it)
		dir.rmdir(*it);
	dir.rmdir(tmppath);
	archivename = QString::null;
}

void ImgArchiveSink::create(const QString &destname, QValueList<int> pages)
{
	//TODO
	for (QValueList<int>::const_iterator it = pages.begin(); it != pages.end(); it++)
		std::cout << *it << " ";
	std::cout << "\n";
}

QString ImgArchiveSink::getName(int maxlen)
{
	if (archivename.length() < maxlen)
		return archivename;
	QString tmpname = archivename.left(maxlen-3) + "...";
	return tmpname;
}

QString ImgArchiveSink::getFullName()
{
	return archivepath;
}

void ImgArchiveSink::infoExited()
{
	extcnt = 0;
	if (!pext->start())
		emit sinkError(SINKERR_OTHER);
}

void ImgArchiveSink::extractExited()
{
	//
	// open temporary directory using ImgDirSink::open()
	ImgDirSink::blockSignals(true);
	int status = ImgDirSink::open(tmppath);
	ImgDirSink::blockSignals(false);

	archfiles = ImgDirSink::getAllfiles();
	archdirs = ImgDirSink::getAlldirs();
	setComicBookName(archivename);

	if (!pext->normalExit())
	{
		emit sinkError(SINKERR_ARCHEXIT);
		close();
	}
	else if (status != 0)
	{
		emit sinkError(status);
		close();
	}
	else
	{
		emit progress(1, 1);
		//
		// fix permissions of files; this is needed for ace archives as unace
		// is buggy and sets empty permissions.
		for (QStringList::const_iterator it = archfiles.begin(); it!=archfiles.end(); ++it)
		{
			QFileInfo finfo(*it);
			if (!finfo.isReadable())
				chmod(*it, S_IRUSR|S_IWUSR);
		}
		emit sinkReady(archivepath);
	}
}

void ImgArchiveSink::infoStdoutReady()
{
	QByteArray b = pinf->readStdout();
	for (int i=0; i<b.size(); i++)
		if (b[i] == '\n')
			++filesnum;
}

void ImgArchiveSink::extractStdoutReady()
{
	QByteArray b = pext->readStdout();
	for (int i=0; i<b.size(); i++)
		if (b[i] == '\n' && extcnt < filesnum)
			++extcnt;
	emit progress(extcnt, filesnum);
	qApp->processEvents();
}

void ImgArchiveSink::autoconfRAR()
{
	ArchiveTypeInfo inf;
	inf.type = RAR_ARCHIVE;
	inf.name = "rar";
	inf.extensions.append(".rar");
	inf.extensions.append(".cbr");
	inf.reading = inf.writing = false;

	if (which("rar") != QString::null)
	{
		inf.extractopts.append("rar");
		inf.extractopts.append("x");
		inf.listopts.append("rar");
		inf.listopts.append("lb");
		inf.reading = inf.writing = true;
		return;
	}
	if (which("unrar") != QString::null)
	{
		inf.extractopts.append("unrar");
		inf.extractopts.append("x");
		inf.listopts.append("unrar");
		inf.listopts.append("lb");
		inf.name = "rar";
		inf.reading = true;
	}
	archinfo.append(inf);
}

void ImgArchiveSink::autoconfZIP()
{
	ArchiveTypeInfo inf;
	inf.type = ZIP_ARCHIVE;
	inf.name = "zip";
	inf.extensions.append(".zip");
	inf.extensions.append(".cbz");
	inf.reading = inf.writing = false;
	if (which("unzip") != QString::null)
	{
		inf.extractopts.append("unzip");
		inf.listopts.append("unzip");
		inf.listopts.append("-l");
		inf.reading = true;
	}
	if (which("zip") != QString::null)
	{
		inf.writing = true;
	}
	archinfo.append(inf);
}

void ImgArchiveSink::autoconfACE()
{
	ArchiveTypeInfo inf;
	inf.type = ACE_ARCHIVE;
	inf.name = "ace";
	inf.extensions.append(".ace");
	inf.extensions.append(".cba");
	inf.reading = inf.writing = false;
	if (which("unace") != QString::null)
	{
		inf.extractopts.append("unace");
		inf.extractopts.append("x");
		inf.extractopts.append("-y");
		inf.extractopts.append("-c-");
		inf.listopts.append("unace");
		inf.listopts.append("l");
		inf.listopts.append("-y");
		inf.listopts.append("-c-");
		inf.reading = true;
	}
	archinfo.append(inf);
}

void ImgArchiveSink::autoconfTARGZ()
{
	ArchiveTypeInfo inf;
	inf.type = TARGZ_ARCHIVE;
	inf.name = "tar.gz";
	inf.extensions.append(".tar.gz");
	inf.extensions.append(".tgz");
	inf.extensions.append(".cbg");
	inf.reading = inf.writing = false;
	if (which("tar") != QString::null)
	{
		inf.extractopts.append("tar");
		inf.extractopts.append("-xvzf");
		inf.listopts.append("tar");
		inf.listopts.append("-tzf");
		inf.reading = inf.writing = true;
	}
	archinfo.append(inf);
}

void ImgArchiveSink::autoconfTARBZ2()
{
	ArchiveTypeInfo inf;
	inf.type = TARBZ2_ARCHIVE;
	inf.name = "tar.bz2";
	inf.extensions.append(".tar.bz2");
	inf.extensions.append(".cbb");
	if (which("tar") != QString::null)
	{
		inf.extractopts.append("tar");
		inf.extractopts.append("-xjvf");
		inf.listopts.append("tar");
		inf.listopts.append("-tjf");
		inf.reading = inf.writing = true;
	}
	archinfo.append(inf);
}

void ImgArchiveSink::autoconfArchivers()
{
	autoconfRAR();
	autoconfZIP();
	autoconfACE();
	autoconfTARGZ();
	autoconfTARBZ2();
	
	openext = saveext = QString::null;	

	for (QValueList<ArchiveTypeInfo>::const_iterator it = archinfo.begin(); it!=archinfo.end(); it++)
	{
		const ArchiveTypeInfo &inf = *it;
		if (inf.reading)
			suppopen |= inf.type;
		if (inf.writing)
			suppsave |= inf.type;
		for (QStringList::const_iterator sit = inf.extensions.begin(); sit!=inf.extensions.end(); sit++)
		{
			if (inf.reading)
			{
				if (openext != QString::null)
					openext += " ";
				openext += "*" + *sit;
			}
			if (inf.writing)
			{
				if (saveext != QString::null)
					saveext += " ";
				saveext += "*" + *sit;
			}
		}
	}
}

QString ImgArchiveSink::supportedOpenExtensions()
{
	return openext;
}

QString ImgArchiveSink::supportedSaveExtensions()
{
	return saveext;
}

int ImgArchiveSink::supportedArchives()
{
	return suppopen;
}

QValueList<ImgArchiveSink::ArchiveTypeInfo> ImgArchiveSink::supportedArchivesInfo()
{
	return archinfo;
}

bool ImgArchiveSink::supportsOpen(ArchiveType t)
{
	return (suppopen & t) > 0;
}

bool ImgArchiveSink::supportsSave(ArchiveType t)
{
	return (suppsave & t) > 0;
}

