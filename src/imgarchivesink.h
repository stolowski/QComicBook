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

#ifndef __IMGARCHIVESINK_H
#define __IMGARCHIVESINK_H

#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include "imgdirsink.h"

class QImage;
class QProcess;

class ImgArchiveSink: public ImgDirSink
{
	Q_OBJECT

	protected:
	
		enum ArchiveType { RAR_ARCHIVE = 0, ZIP_ARCHIVE, UNKNOWN_ARCHIVE };

		QProcess *pext; //extracting process
		QString archivename; //archive file name, without path
		QString archivepath; //full path, including archive name
		QString tmppath; //path to extracted archive
		QStringList archfiles; //list of archive files
		QStringList archdirs; //list of archive dirs
		
		static QStringList zip; //unzip executable and options
		static QStringList rar; //rar/unrar executbale and options
		static bool havezip;
		static bool haverar;

		static ArchiveType archiveType(const QString &filename);
		int extract(const QString &filename, const QString &destdir);
		
	private slots:
		void extractExited();

	public:
		ImgArchiveSink(int cachesize=1);
		ImgArchiveSink(const QString &path, int cachesize=1);
		virtual ~ImgArchiveSink();

		virtual int open(const QString &path);
		virtual void close();
		virtual QString getName(int maxlen = 50);
		virtual QString getFullName();

		static bool autoconfRAR();
		static bool autoconfZIP();
		static bool haveRAR();
		static bool haveZIP();
};

#endif

