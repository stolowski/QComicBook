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

#include "imgarchivesink.h"
#include "miscutil.h"
#include <QStringList>
#include <QProcess>
#include <QTextStream>
#include <QFileInfo>
#include <QFile>
#include <QRegExp>
#include <QApplication>
#include <QDir>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

using namespace QComicBook;
using Utility::which;

QStringList ImgArchiveSink::openext;
QStringList ImgArchiveSink::saveext;
int ImgArchiveSink::suppopen;
int ImgArchiveSink::suppsave;

QList<ImgArchiveSink::ArchiveTypeInfo> ImgArchiveSink::archinfo;

ImgArchiveSink::ImgArchiveSink(): ImgDirSink()
{
	init();
}

ImgArchiveSink::ImgArchiveSink(const QString &path): ImgDirSink()
{
	init();
	open(path);
}

ImgArchiveSink::ImgArchiveSink(const ImgDirSink &sink): ImgDirSink(sink)
{
	init();
}

ImgArchiveSink::~ImgArchiveSink()
{
	ImgArchiveSink::close();
}

bool ImgArchiveSink::fileHandler(const QFileInfo &finfo)
{
	const QString fname = finfo.fileName();
	const QString fullname = finfo.absoluteFilePath();
	std::cout << fullname.toStdString() << std::endl;

	if (finfo.isDir())
		archdirs.prepend(fullname);

	if (ImgDirSink::fileHandler(finfo))
	{
		archfiles.append(fullname);
		return true;
	}

	if (finfo.isFile())
	{
		ArchiveType archiveType = getArchiveType(fullname);
		if (archiveType != UNKNOWN_ARCHIVE)
		{
			std::cout << "got nested archive, extracting it!" << std::endl;
			const QString tmp = makeTempDir(finfo.absolutePath());
			archdirs.prepend(tmp);
			extract(fullname, tmp, archiveType); //TODO: errors

			//
			// remove cb archive we just extracted, no need to waste space
			QDir dir(finfo.absolutePath());
			dir.remove(fname);
			visit(tmp);

			return true;
		}
		archfiles.append(fullname);
	}

	return false;
}

void ImgArchiveSink::init()
{
	pinf = new QProcess(this);
	pext = new QProcess(this);
	connect(pinf, SIGNAL(readyReadStandardOutput()), this, SLOT(infoStdoutReady()));
	connect(pinf, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(infoExited(int, QProcess::ExitStatus)));
	connect(pext, SIGNAL(readyReadStandardOutput()), this, SLOT(extractStdoutReady()));
	connect(pext, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(extractExited(int, QProcess::ExitStatus)));
}

void ImgArchiveSink::doCleanup()
{
	if (!tmppath.isEmpty())
	{
		QDir dir(tmppath);
		//
		// remove temporary files and dirs
		foreach (const QString f, archfiles)
			dir.remove(f);
		foreach (const QString f, archdirs)
		{
			std::cout << f.toStdString() << std::endl;
			dir.rmdir(f);
		}
		dir.rmdir(tmppath);
	}
}

ImgArchiveSink::ArchiveType ImgArchiveSink::getArchiveType(const QString &filename)
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
	if (file.open(QIODevice::ReadOnly))
	{
		for (int i=0; i<3; i++)
		{
			int j;
			if (!file.seek(magic[i].offset))
				continue;
			for (j=0; j<magic[i].len; j++)
			{
				char c;
				if (!file.getChar(&c))
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
	foreach (const ArchiveTypeInfo inf, archinfo)
	{
		foreach (const QString s, inf.extensions)
		{
			if (filename.endsWith(s, Qt::CaseInsensitive))
				return inf.type;
		}
	}

	return UNKNOWN_ARCHIVE;
}

int ImgArchiveSink::extract(const QString &filename, const QString &destdir, ArchiveType archiveType)
{
	if (archiveType== UNKNOWN_ARCHIVE)
		archiveType = getArchiveType(filename);

	if (archiveType == UNKNOWN_ARCHIVE)
		return SINKERR_UNKNOWNFILE;
	if (!supportsOpen(archiveType))
		return SINKERR_NOTSUPPORTED;	

	const QFileInfo finf(filename);
	if (!finf.isReadable())
		return SINKERR_ACCESS;

	std::cout << "extract: " << filename.toStdString() << " dir=" << destdir.toStdString() << std::endl;

	QStringList extargs;
	QStringList infargs;
	QString extprg;
	QString infprg;

	//
	// match archive type, set subprocess extract and list options
	foreach (const ArchiveTypeInfo inf, archinfo)
	{
		if (archiveType == inf.type)
		{
			extargs = inf.extractopts;
			extprg = extargs.takeFirst();
			infargs = inf.listopts;
			infprg = infargs.takeFirst();
			break;
		}
	}
	
	extargs << filename;
	infargs << filename;
	pext->setWorkingDirectory(destdir);

	//
	// extract archive file list first
	pinf->start(infprg, infargs);
	if (!pinf->waitForFinished(-1))
		return SINKERR_ARCHEXIT;
	
	extcnt = 0;
	pext->start(extprg, extargs);
	if (!pext->waitForFinished(-1))
		return SINKERR_ARCHEXIT;

	return 0;
}

int ImgArchiveSink::open(const QString &path) //TODO: cleanup if already opened?
{
	filesnum = 0;
	archfiles.clear();

	QFileInfo info(path);
	archivepath = path;
	archivename = info.fileName();
	if (!info.exists())
		return SINKERR_NOTFOUND;
	if (info.isFile())
	{
		if (info.isReadable())
		{
			tmppath = makeTempDir();
			archdirs.prepend(tmppath);
			int status = extract(path, tmppath);
			if (status != 0)
			{
				close();
				return status;
			}
			visit(tmppath);
			emit progress(1, 1);
			setComicBookName(archivename);
			return 0;
		}
		else
		{
			return SINKERR_ACCESS;
		}
	}
	return SINKERR_NOTFILE;
}

void ImgArchiveSink::close()
{
	ImgDirSink::close();
	doCleanup();
	archivename = QString::null;
}

QString ImgArchiveSink::getName(int maxlen)
{
	if (archivename.length() < maxlen)
		return archivename;
	QString tmpname = archivename.left(maxlen-3) + "...";
	return tmpname;
}

QString ImgArchiveSink::getFullName() const
{
	return archivepath;
}

void ImgArchiveSink::infoExited(int code, QProcess::ExitStatus exitStatus)
{
}

void ImgArchiveSink::extractExited(int code, QProcess::ExitStatus exitStatus)
{
	//
	// fix permissions of files; this is needed for ace archives as unace
	// is buggy and sets empty permissions.
	foreach (const QString f, archfiles)
	{
		QFileInfo finfo(f);
		if (!finfo.isReadable())
			chmod(f.toLocal8Bit(), S_IRUSR|S_IWUSR);
	}
}

void ImgArchiveSink::infoStdoutReady()
{
	QByteArray b = pinf->readAllStandardOutput();
	for (int i=0; i<b.size(); i++)
		if (b[i] == '\n')
			++filesnum;
}

void ImgArchiveSink::extractStdoutReady()
{
	QByteArray b = pext->readAllStandardOutput();
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
		inf.compressopts.append("rar");
		inf.compressopts.append("a");
	}
	else if (which("unrar") != QString::null)
	{
		FILE *f;
		bool nonfree_unrar = false;
		inf.extractopts.append("unrar");
		inf.listopts.append("unrar");
		//
		// now determine which unrar it is - free or non-free
		if ((f = popen("unrar", "r")) != NULL)
		{
			QRegExp regexp("^UNRAR.+freeware");
			for (QTextStream s(f); !s.atEnd(); )
			{
				if (regexp.indexIn(s.readLine()) >= 0)
				{
					nonfree_unrar = true;
					break;
				}
			}
			pclose(f);
			if (nonfree_unrar)
			{
				inf.extractopts.append("x");
				inf.listopts.append("lb");
			}
			else
			{
				inf.extractopts.append("-x");
				inf.listopts.append("-t");
			}
			inf.reading = true;
		}
	}
	else if (which("unrar-free") != QString::null) //some distros rename free unrar like this
	{
		inf.extractopts.append("unrar-free");
		inf.listopts.append("unrar-free");
		inf.extractopts.append("-x");
		inf.listopts.append("-t");
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
		inf.compressopts.append("zip");
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
		inf.compressopts.append("tar");
		inf.compressopts.append("-chzvf");
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
		inf.compressopts.append("tar");
		inf.compressopts.append("-chjvf");
	}
	archinfo.append(inf);
}

void ImgArchiveSink::autoconfSEVENZIP()
{
	ArchiveTypeInfo inf;
	inf.type = SEVENZIP_ARCHIVE;
	inf.name = "7z";
	inf.extensions.append(".7z");
	if (which("7z") != QString::null)
	{
		inf.extractopts.append("7z");
		inf.extractopts.append("x");
		inf.listopts.append("7z");
		inf.listopts.append("l");
		inf.reading = true;
		inf.writing = false;
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
	autoconfSEVENZIP();
	
	openext.clear();
	saveext.clear();

	foreach (const ArchiveTypeInfo inf, archinfo)
	{
		if (inf.reading)
			suppopen |= inf.type;
		if (inf.writing)
			suppsave |= inf.type;
		foreach (const QString ext, inf.extensions)
		{
			if (inf.reading)
				openext.append("*" + ext);
			if (inf.writing)
				saveext.append("*" + ext);
		}
	}
}

QString ImgArchiveSink::makeTempDir(const QString &parent)
{
	char tmpd[1024];
	const QString pattern = parent + QDir::separator() + "qcomic-XXXXXX";
	strcpy(tmpd, pattern.toLatin1());
	mkdtemp(tmpd);
	return QString(tmpd);
}

QStringList ImgArchiveSink::supportedOpenExtensions()
{
	return openext;
}

QStringList ImgArchiveSink::supportedSaveExtensions()
{
	return saveext;
}

int ImgArchiveSink::supportedArchives()
{
	return suppopen;
}

QList<ImgArchiveSink::ArchiveTypeInfo> ImgArchiveSink::supportedArchivesInfo()
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

bool ImgArchiveSink::supportsNext() const
{
	return true;
}

QString ImgArchiveSink::getNext() const
{
	QFileInfo finfo(getFullName());
	QDir dir(finfo.absolutePath()); //get the full path of current cb
	QStringList files = dir.entryList(ImgArchiveSink::supportedOpenExtensions(), QDir::Files|QDir::Readable, QDir::Name);
	int i = files.indexOf(finfo.fileName()); //find current cb
	if ((i > 0) && (i < files.size()-1))
		return dir.absoluteFilePath(files.at(i+1));  //get next file name
	return QString::null;
}

QString ImgArchiveSink::getPrevious() const
{
	QFileInfo finfo(getFullName());
	QDir dir(finfo.absolutePath()); //get the full path of current cb
	QStringList files = dir.entryList(ImgArchiveSink::supportedOpenExtensions(), QDir::Files|QDir::Readable, QDir::Name);
	int i = files.indexOf(finfo.fileName()); //find current cb
	if (i > 0)
		return dir.absoluteFilePath(files.at(i-1));
	return QString::null;
}

