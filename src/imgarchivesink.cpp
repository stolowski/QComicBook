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
#include <qdatastream.h>
#include <stdlib.h>

QStringList ImgArchiveSink::zip;
QStringList ImgArchiveSink::rar;
bool ImgArchiveSink::havezip = false;
bool ImgArchiveSink::haverar = false;

ImgArchiveSink::ImgArchiveSink(int cachesize): ImgDirSink(cachesize) /*{{{*/
{
}/*}}}*/

ImgArchiveSink::ImgArchiveSink(const QString &path, int cachesize): ImgDirSink(cachesize)/*{{{*/
{
	open(path);
}/*}}}*/

ImgArchiveSink::~ImgArchiveSink()/*{{{*/
{
	close();
}/*}}}*/

ImgArchiveSink::ArchiveType ImgArchiveSink::archiveType(const QString &filename)/*{{{*/
{
	static const unsigned char magic[2][5] = {
		{0x52, 0x61, 0x72, 0x21, RAR_ARCHIVE},
		{0x50, 0x4b, 0x03, 0x04, ZIP_ARCHIVE}
	};

	ArchiveType archivetype = UNKNOWN_ARCHIVE;

	QFile file(filename);
	if (file.open(IO_ReadOnly))
	{
		char buf[4];
		QDataStream stream(&file);
		stream.readRawBytes(buf, 4);
		file.close();
		for (int i=0; i<2; i++)
		{
			int j;
			for (j=0; j<4 && buf[j]==magic[i][j]; j++);
			if (j == 4)
			{
				archivetype = static_cast<ArchiveType>(magic[i][4]);
				break;
			}
		}
	}
	return archivetype;
}/*}}}*/

int ImgArchiveSink::extract(const QString &filename, const QString &destdir)/*{{{*/
{
	ArchiveType archivetype = archiveType(filename);
		
	pext = new QProcess(this);
	connect(pext, SIGNAL(processExited()), this, SLOT(extractExited()));
	
	if (archivetype == RAR_ARCHIVE)
	{
		pext->setArguments(rar);
	}
	else if (archivetype == ZIP_ARCHIVE)
	{
		pext->setArguments(zip);
	}
	else
		return SINKERR_UNKNOWNFILE;

	pext->addArgument(filename);
	pext->setWorkingDirectory(destdir);
	pext->start();

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
			char tmpd[255];
			strcpy(tmpd, "/tmp/qcomic-XXXXXX");
			mkdtemp(tmpd);
			int status = extract(path, tmppath = tmpd);
			if (status != 0)
				close();
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
	QDir dir(tmppath);
	//
	// remove temporary files and dirs
	for (QStringList::const_iterator it = archfiles.begin(); it != archfiles.end(); ++it)
		dir.remove(*it);
	for (QStringList::const_iterator it = archdirs.begin(); it != archdirs.end(); ++it)
		dir.rmdir(*it);
	dir.rmdir(tmppath);
	archivename = QString::null;
	ImgDirSink::close();
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

void ImgArchiveSink::extractExited()/*{{{*/
{
	pext->deleteLater();
	
	//
	// open temporary directory using ImgDirSink::open()
	ImgDirSink::blockSignals(true);
	int status = ImgDirSink::open(tmppath);
	ImgDirSink::blockSignals(false);

	archfiles = ImgDirSink::getAllfiles();
	archdirs = ImgDirSink::getAlldirs();
	setComicBookName(archivename);

	if (status != 0)
	{
		emit sinkError(status);
		close();
	}
	else
		emit sinkReady(archivepath);
}/*}}}*/

bool ImgArchiveSink::autoconfRAR()/*{{{*/
{
	rar.clear();
	if (which("rar"))
	{
		rar.append("rar");
		rar.append("x");
		return haverar = true;
	}
	if (which("unrar"))
	{
		rar.append("unrar");
		rar.append("x");
		return haverar = true;
	}
	return haverar = false;
}/*}}}*/

bool ImgArchiveSink::autoconfZIP()/*{{{*/
{
	zip.clear();
	if (which("unzip"))
	{
		zip.append("unzip");
		return havezip = true;
	}
	return havezip = false;
}/*}}}*/

bool ImgArchiveSink::haveRAR()/*{{{*/
{
	return haverar;
}/*}}}*/

bool ImgArchiveSink::haveZIP()/*{{{*/
{
	return havezip;
}/*}}}*/

