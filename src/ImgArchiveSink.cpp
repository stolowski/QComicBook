/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "ImgArchiveSink.h"
#include "Utility.h"
#include "ArchiversConfiguration.h"
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
#include <time.h>

using namespace QComicBook;
using Utility::which;

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

	if (finfo.isDir())
		archdirs.prepend(fullname);

	if (ImgDirSink::fileHandler(finfo))
	{
		archfiles.append(fullname);
		return true;
	}

	if (finfo.isFile())
	{
            QStringList extractargs, listargs;
            ArchiversConfiguration::instance().getExtractArguments(fullname, extractargs, listargs);
            if (extractargs.size())
            {
                const QString tmp = makeTempDir(finfo.absolutePath());
                archdirs.prepend(tmp);
                extract(fullname, tmp, extractargs, listargs);
                
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
			dir.rmdir(f);
		}
		dir.rmdir(tmppath);
	}
}

int ImgArchiveSink::waitForFinished(QProcess *p)
{
        //
        // an ugly busy-loop with 1s sleep.
        // waitForFinished(-1) hangs when used with processEvents().
        for (;;)
        {
            if (p->waitForFinished(1000) == true)
                break;
            if (p->state() == QProcess::NotRunning)
                break;
        }
        return p->exitStatus() == QProcess::NormalExit && p->exitCode() == 0;
}

int ImgArchiveSink::extract(const QString &filename, const QString &destdir, QStringList extargs, QStringList infargs)
{
    if (extargs.size() == 0 )
		return SINKERR_UNKNOWNFILE;
    
    const QFileInfo finf(filename);
    if (!finf.isReadable())
        return SINKERR_ACCESS;
    
    const QString extprg = extargs.takeFirst();
    const QString infprg = infargs.takeFirst();
    
    pext->setWorkingDirectory(destdir);
    
    //
    // extract archive file list first
    pinf->start(infprg, infargs);
    
    if (!waitForFinished(pinf))
        return SINKERR_ARCHEXIT;
    
    extcnt = 0;
    pext->start(extprg, extargs);
    return waitForFinished(pext) ? 0 : SINKERR_ARCHEXIT;
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
                        QStringList extractargs, listargs;
                        ArchiversConfiguration::instance().getExtractArguments(path, extractargs, listargs);
			int status = extract(path, tmppath, extractargs, listargs);
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
	qApp->processEvents(QEventLoop::ExcludeSocketNotifiers | QEventLoop::ExcludeUserInputEvents);
}

QString ImgArchiveSink::makeTempDir(const QString &parent)
{
#ifdef Q_WS_WIN
	const QString pattern = parent + QDir::separator() + "qcomic-";
	srand(time(NULL));
	int number = 0;
	QString tmpd = pattern + QString::number(number = rand());
	while (QFileInfo(tmpd).exists())
	{
		QString tmpd = pattern + QString::number(number = rand());
	}
	QDir dir(parent);
	dir.mkdir(QString("qcomic-") + QString::number(number));
	return tmpd;
#else
	char tmpd[1024];
	const QString pattern = parent + QDir::separator() + "qcomic-XXXXXX";
	strcpy(tmpd, pattern.toLatin1());
	mkdtemp(tmpd);
	return QString(tmpd);
#endif
}

bool ImgArchiveSink::supportsNext() const
{
	return true;
}

QString ImgArchiveSink::getNext() const
{
	QFileInfo finfo(getFullName());
	QDir dir(finfo.absolutePath()); //get the full path of current cb
	QStringList files = dir.entryList(ArchiversConfiguration::instance().supportedOpenExtensions(), QDir::Files|QDir::Readable, QDir::Name);
	int i = files.indexOf(finfo.fileName()); //find current cb
	if ((i > 0) && (i < files.size()-1))
		return dir.absoluteFilePath(files.at(i+1));  //get next file name
	return QString::null;
}

QString ImgArchiveSink::getPrevious() const
{
	QFileInfo finfo(getFullName());
	QDir dir(finfo.absolutePath()); //get the full path of current cb
	QStringList files = dir.entryList(ArchiversConfiguration::instance().supportedOpenExtensions(), QDir::Files|QDir::Readable, QDir::Name);
	int i = files.indexOf(finfo.fileName()); //find current cb
	if (i > 0)
		return dir.absoluteFilePath(files.at(i-1));
	return QString::null;
}

