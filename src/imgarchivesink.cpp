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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

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
bool ImgArchiveSink::havezip = false;
bool ImgArchiveSink::haverar = false;
bool ImgArchiveSink::haveace = false;
bool ImgArchiveSink::havetargz = false;
bool ImgArchiveSink::havetarbz2 = false;

ImgArchiveSink::ImgArchiveSink(int cachesize): ImgDirSink(cachesize) /*{{{*/
{
	init();
}/*}}}*/

ImgArchiveSink::ImgArchiveSink(const QString &path, int cachesize): ImgDirSink(cachesize)/*{{{*/
{
	init();
	open(path);
}/*}}}*/

ImgArchiveSink::~ImgArchiveSink()/*{{{*/
{
	close();
}/*}}}*/

void ImgArchiveSink::init()/*{{{*/
{
	connect(&pinf, SIGNAL(readyReadStdout()), this, SLOT(infoStdoutReady()));
	connect(&pinf, SIGNAL(processExited()), this, SLOT(infoExited()));
	connect(&pext, SIGNAL(readyReadStdout()), this, SLOT(extractStdoutReady()));
	connect(&pext, SIGNAL(processExited()), this, SLOT(extractExited()));
}/*}}}*/

ImgArchiveSink::ArchiveType ImgArchiveSink::archiveType(const QString &filename)/*{{{*/
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

	if (filename.endsWith(".tar.gz", false) || filename.endsWith(".tgz", false) || filename.endsWith(".cbg", false))
		return TARGZ_ARCHIVE;
	if (filename.endsWith(".tar.bz2", false) || filename.endsWith(".cbb", false))
		return TARBZ2_ARCHIVE;
	if (filename.endsWith(".rar", false) || filename.endsWith(".cbr", false))
		return RAR_ARCHIVE;
	if (filename.endsWith(".zip", false) || filename.endsWith(".cbz", false))
		return ZIP_ARCHIVE;
	if (filename.endsWith(".ace", false) || filename.endsWith(".cba", false))
		return ACE_ARCHIVE;

	return UNKNOWN_ARCHIVE;
}/*}}}*/

int ImgArchiveSink::extract(const QString &filename, const QString &destdir)/*{{{*/
{
	archivetype = archiveType(filename);
	filesnum = 0;
	
	if (archivetype == UNKNOWN_ARCHIVE)
		return SINKERR_UNKNOWNFILE;

	if ((archivetype == RAR_ARCHIVE && !haverar) || (archivetype == ZIP_ARCHIVE && !havezip) ||
	    (archivetype == ACE_ARCHIVE && !haveace) || (archivetype == TARGZ_ARCHIVE && !havetargz) ||
	    (archivetype == TARBZ2_ARCHIVE && !havetarbz2))
		return SINKERR_NOTSUPPORTED;	
		
	if (archivetype == RAR_ARCHIVE)
	{
		pext.setArguments(rar);
		pinf.setArguments(rar_i);
	}
	else if (archivetype == ZIP_ARCHIVE)
	{
		pext.setArguments(zip);
		pinf.setArguments(zip_i);
	}
	else if (archivetype == ACE_ARCHIVE)
	{
		pext.setArguments(ace);
		pinf.setArguments(ace_i);
	}
	else if (archivetype == TARGZ_ARCHIVE)
	{
		pext.setArguments(targz);
		pinf.setArguments(targz_i);
	}
	else if (archivetype == TARBZ2_ARCHIVE)
	{
		pext.setArguments(tarbz2);
		pinf.setArguments(tarbz2_i);
	}

	pext.addArgument(filename);
	pinf.addArgument(filename);
	pext.setWorkingDirectory(destdir);

	emit progress(0, 1);

	//
	// extract archive file list first
	if (!pinf.start())
		return SINKERR_OTHER;
	return 0;
}/*}}}*/

int ImgArchiveSink::open(const QString &path)/*{{{*/
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
}/*}}}*/

void ImgArchiveSink::close()/*{{{*/
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
}/*}}}*/

QString ImgArchiveSink::getName(int maxlen)/*{{{*/
{
	if (archivename.length() < maxlen)
		return archivename;
	QString tmpname = archivename.left(maxlen-3) + "...";
	return tmpname;
}/*}}}*/

QString ImgArchiveSink::getFullName()/*{{{*/
{
	return archivepath;
}/*}}}*/

void ImgArchiveSink::infoExited()/*{{{*/
{
	extcnt = 0;
	if (!pext.start())
		emit sinkError(SINKERR_OTHER);
}/*}}}*/

void ImgArchiveSink::extractExited()/*{{{*/
{
	//
	// open temporary directory using ImgDirSink::open()
	ImgDirSink::blockSignals(true);
	int status = ImgDirSink::open(tmppath);
	ImgDirSink::blockSignals(false);

	archfiles = ImgDirSink::getAllfiles();
	archdirs = ImgDirSink::getAlldirs();
	setComicBookName(archivename);

	if (!pext.normalExit())
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
}/*}}}*/

void ImgArchiveSink::infoStdoutReady()/*{{{*/
{
	QByteArray b = pinf.readStdout();
	for (int i=0; i<b.size(); i++)
		if (b[i] == '\n')
			++filesnum;
}/*}}}*/

void ImgArchiveSink::extractStdoutReady()/*{{{*/
{
	QByteArray b = pext.readStdout();
	for (int i=0; i<b.size(); i++)
		if (b[i] == '\n' && extcnt < filesnum)
			emit progress(++extcnt, filesnum);
}/*}}}*/

bool ImgArchiveSink::autoconfRAR()/*{{{*/
{
	rar.clear();
	rar_i.clear();
	if (which("rar"))
	{
		rar.append("rar");
		rar.append("x");
		rar_i.append("rar");
		rar_i.append("lb");
		return haverar = true;
	}
	if (which("unrar"))
	{
		rar.append("unrar");
		rar.append("x");
		rar_i.append("unrar");
		rar_i.append("lb");
		return haverar = true;
	}
	return haverar = false;
}/*}}}*/

bool ImgArchiveSink::autoconfZIP()/*{{{*/
{
	zip.clear();
	zip_i.clear();
	if (which("unzip"))
	{
		zip.append("unzip");
		zip_i.append("unzip");
		zip_i.append("-l");
		return havezip = true;
	}
	return havezip = false;
}/*}}}*/

bool ImgArchiveSink::autoconfACE()/*{{{*/
{
	ace.clear();
	ace_i.clear();
	if (which("unace"))
	{
		ace.append("unace");
		ace.append("x");
		ace.append("-y");
		ace.append("-c-");
		ace_i.append("unace");
		ace_i.append("l");
		ace_i.append("-y");
		ace_i.append("-c-");
		return haveace = true;
	}
	return haveace = false;
}/*}}}*/

bool ImgArchiveSink::autoconfTARGZ()/*{{{*/
{
	targz.clear();
	targz_i.clear();
	if (which("tar"))
	{
		targz.append("tar");
		targz.append("-xvzf");
		targz_i.append("tar");
		targz_i.append("-tzf");
		return havetargz = true;
	}
	return havetargz = false;
}/*}}}*/

bool ImgArchiveSink::autoconfTARBZ2()/*{{{*/
{
	tarbz2.clear();
	tarbz2_i.clear();
	if (which("tar"))
	{
		tarbz2.append("tar");
		tarbz2.append("-xjvf");
		tarbz2_i.append("tar");
		tarbz2_i.append("-tjf");
		return havetarbz2 = true;
	}
	return havetarbz2 = false;
}/*}}}*/

bool ImgArchiveSink::haveRAR()/*{{{*/
{
	return haverar;
}/*}}}*/

bool ImgArchiveSink::haveZIP()/*{{{*/
{
	return havezip;
}/*}}}*/

bool ImgArchiveSink::haveACE()/*{{{*/
{
	return haveace;
}/*}}}*/

bool ImgArchiveSink::haveTARGZ()/*{{{*/
{
	return havetargz;
}/*}}}*/

bool ImgArchiveSink::haveTARBZ2()/*{{{*/
{
	return havetarbz2;
}/*}}}*/

