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
QStringList ImgArchiveSink::zipi;
QStringList ImgArchiveSink::rari;
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
	filesnum = 0;
		
	pinf = new QProcess(this);
	pinf->setCommunication(QProcess::Stdout);
	connect(pinf, SIGNAL(readyReadStdout()), this, SLOT(infoStdoutReady()));
	connect(pinf, SIGNAL(processExited()), this, SLOT(infoExited()));
	pext = new QProcess(this);
	pext->setCommunication(QProcess::Stdout);
	connect(pext, SIGNAL(readyReadStdout()), this, SLOT(extractStdoutReady()));
	connect(pext, SIGNAL(processExited()), this, SLOT(extractExited()));
	
	if (archivetype == RAR_ARCHIVE)
	{
		pext->setArguments(rar);
		pinf->setArguments(rari);
	}
	else if (archivetype == ZIP_ARCHIVE)
	{
		pext->setArguments(zip);
		pinf->setArguments(zipi);
	}
	else
		return SINKERR_UNKNOWNFILE;

	pext->addArgument(filename);
	pinf->addArgument(filename);
	pext->setWorkingDirectory(destdir);

	//
	// extract archive file list first. wait until the process is running
	if (!pinf->start())
	{
		delete pinf;
		delete pext;
		emit sinkError(SINKERR_OTHER);
	}
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

void ImgArchiveSink::infoExited()/*{{{*/
{
	extcnt = 0;
	pinf->deleteLater();
	pext->start();
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

void ImgArchiveSink::infoStdoutReady()/*{{{*/
{
	QByteArray b = pinf->readStdout();
	for (int i=0; i<b.size(); i++)
		if (b[i] == '\n')
			filesnum++;
}/*}}}*/

void ImgArchiveSink::extractStdoutReady()/*{{{*/
{
	QByteArray b = pext->readStdout();
	for (int i=0; i<b.size(); i++)
		if (b[i] == '\n')
			emit progress(++extcnt, filesnum);
	/*if (pext->canReadLineStdout())
	{
		QString l = pext->readLineStdout();
		if (l.find("Extracting") == -1 || l.find("inflating") == -1)
			emit progress(++extcnt, filesnum);
	}*/
}/*}}}*/

bool ImgArchiveSink::autoconfRAR()/*{{{*/
{
	rar.clear();
	if (which("rar"))
	{
		rar.append("rar");
		rar.append("x");
		rari.append("rar");
		rari.append("lb");
		return haverar = true;
	}
	if (which("unrar"))
	{
		rar.append("unrar");
		rar.append("x");
		rari.append("unrar");
		rari.append("lb");
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
		zipi.append("unzip");
		zipi.append("-l");
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

