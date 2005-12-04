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

using namespace QComicBook;
using Utility::which;

QStringList ImgArchiveSink::zip;
QStringList ImgArchiveSink::rar;
QStringList ImgArchiveSink::ace;
QStringList ImgArchiveSink::targz;
QStringList ImgArchiveSink::tarbz2;
QStringList ImgArchiveSink::zip_i;
QStringList ImgArchiveSink::rar_i;
QStringList ImgArchiveSink::ace_i;
QStringList ImgArchiveSink::targz_i;
QStringList ImgArchiveSink::tarbz2_i;
int ImgArchiveSink::suppopen = 0;
int ImgArchiveSink::suppsave = 0;
QString ImgArchiveSink::openext;
QString ImgArchiveSink::saveext;

ImgArchiveSink::ArchiveExtension ImgArchiveSink::exts[] = {
				{RAR_ARCHIVE,   ".rar"},
				{RAR_ARCHIVE,   ".cbr"},
				{ZIP_ARCHIVE,   ".zip"},
				{ZIP_ARCHIVE,   ".cbz"},
				{ACE_ARCHIVE,   ".ace"},
				{ACE_ARCHIVE,   ".cba"},
				{TARGZ_ARCHIVE, ".tgz"},
				{TARGZ_ARCHIVE, ".tar.gz"},
				{TARGZ_ARCHIVE, ".cbg"},
				{TARBZ2_ARCHIVE,".tar.bz2"},
				{TARBZ2_ARCHIVE,".cbb"},
				{UNKNOWN_ARCHIVE, QString::null}};

ImgArchiveSink::ImgArchiveSink(int cachesize): ImgDirSink(cachesize) 
{
	init();
}

ImgArchiveSink::ImgArchiveSink(const QString &path, int cachesize): ImgDirSink(cachesize)
{
	init();
	open(path);
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

	for (int i=0; exts[i].archtype!=UNKNOWN_ARCHIVE; i++)
		if (filename.endsWith(exts[i].ext, false))
			return exts[i].archtype;

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
		
	if (archivetype == RAR_ARCHIVE)
	{
		pext->setArguments(rar);
		pinf->setArguments(rar_i);
	}
	else if (archivetype == ZIP_ARCHIVE)
	{
		pext->setArguments(zip);
		pinf->setArguments(zip_i);
	}
	else if (archivetype == ACE_ARCHIVE)
	{
		pext->setArguments(ace);
		pinf->setArguments(ace_i);
	}
	else if (archivetype == TARGZ_ARCHIVE)
	{
		pext->setArguments(targz);
		pinf->setArguments(targz_i);
	}
	else if (archivetype == TARBZ2_ARCHIVE)
	{
		pext->setArguments(tarbz2);
		pinf->setArguments(tarbz2_i);
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
	rar.clear();
	rar_i.clear();
	if (which("rar") != QString::null)
	{
		rar.append("rar");
		rar.append("x");
		rar_i.append("rar");
		rar_i.append("lb");
		suppsave |= RAR_ARCHIVE;
		suppopen |= RAR_ARCHIVE;
		return;
	}
	if (which("unrar") != QString::null)
	{
		rar.append("unrar");
		rar.append("x");
		rar_i.append("unrar");
		rar_i.append("lb");
		suppopen |= RAR_ARCHIVE;
	}
}

void ImgArchiveSink::autoconfZIP()
{
	zip.clear();
	zip_i.clear();
	if (which("unzip") != QString::null)
	{
		zip.append("unzip");
		zip_i.append("unzip");
		zip_i.append("-l");
		suppopen |= ZIP_ARCHIVE;
	}
	if (which("zip") != QString::null)
	{
		suppsave |= ZIP_ARCHIVE;
	}
}

void ImgArchiveSink::autoconfACE()
{
	ace.clear();
	ace_i.clear();
	if (which("unace") != QString::null)
	{
		ace.append("unace");
		ace.append("x");
		ace.append("-y");
		ace.append("-c-");
		ace_i.append("unace");
		ace_i.append("l");
		ace_i.append("-y");
		ace_i.append("-c-");
		suppopen |= ACE_ARCHIVE;
	}
}

void ImgArchiveSink::autoconfTARGZ()
{
	targz.clear();
	targz_i.clear();
	if (which("tar") != QString::null)
	{
		targz.append("tar");
		targz.append("-xvzf");
		targz_i.append("tar");
		targz_i.append("-tzf");
		suppopen |= TARGZ_ARCHIVE;
		suppsave |= TARGZ_ARCHIVE;
	}
}

void ImgArchiveSink::autoconfTARBZ2()
{
	tarbz2.clear();
	tarbz2_i.clear();
	if (which("tar") != QString::null)
	{
		tarbz2.append("tar");
		tarbz2.append("-xjvf");
		tarbz2_i.append("tar");
		tarbz2_i.append("-tjf");
		suppopen |= TARBZ2_ARCHIVE;
		suppsave |= TARBZ2_ARCHIVE;
	}
}

void ImgArchiveSink::autoconfArchivers()
{
	autoconfRAR();
	autoconfZIP();
	autoconfACE();
	autoconfTARGZ();
	autoconfTARBZ2();
	
	openext = saveext = QString::null;	
	for (int i=0; exts[i].archtype!=UNKNOWN_ARCHIVE; i++)
	{
		if (suppopen & exts[i].archtype)
		{
			if (openext != QString::null)
				openext += " ";
			openext += "*" + exts[i].ext;
		}
		if (suppsave & exts[i].archtype)
		{
			if (saveext != QString::null)
				saveext += " ";
			saveext += "*" + exts[i].ext;
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

int ImgArchiveSink::supportedCreateArchives()
{
	return suppsave;
}

bool ImgArchiveSink::supportsOpen(ArchiveType t)
{
	return (suppopen & t) > 0;
}

bool ImgArchiveSink::supportsSave(ArchiveType t)
{
	return (suppsave & t) > 0;
}
			
QStringList ImgArchiveSink::getAllExtenstionsList()
{
	QStringList lst; //TODO
	return lst;
}

QString ImgArchiveSink::getExtension(ArchiveType t)
{
	//TODO
}

#include "imgarchivesink.moc"
