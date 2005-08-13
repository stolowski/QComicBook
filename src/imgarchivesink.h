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

/*! \file imgarchivesink.h */

#ifndef __IMGARCHIVESINK_H
#define __IMGARCHIVESINK_H

#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qprocess.h>
#include "imgdirsink.h"

class QImage;

namespace QComicBook
{

	//! Comic book archive sink.
	/*! Allows opening different kind of archives containing image files. */
	class ImgArchiveSink: public ImgDirSink
	{
		Q_OBJECT

		protected:

			enum ArchiveType {
				RAR_ARCHIVE = 0,
				ZIP_ARCHIVE,
				ACE_ARCHIVE,
				TARGZ_ARCHIVE,
				TARBZ2_ARCHIVE,
				UNKNOWN_ARCHIVE
			};

			ArchiveType archivetype; ///< the type of currently opened archive
			QProcess pext; ///< extracting process
			QProcess pinf; ///< file list extracing process
			QString archivename; ///< archive file name, without path
			QString archivepath; ///< full path, including archive name
			QString tmppath; ///< path to extracted archive
			QStringList archfiles; ///< list of archive files
			QStringList archdirs; ///< list of archive dirs
			int filesnum; ///< number of files gathered from parsing archiver output, used for progress bar
			int extcnt; ///< extracted files counter for progress bar

			static QStringList zip; ///< unzip executable and extract options
			static QStringList rar; ///< rar/unrar executbale and extract options
			static QStringList ace; ///< unace executbale and extract options
			static QStringList targz;
			static QStringList tarbz2;
			static QStringList zip_i; ///< unzip executable and list options
			static QStringList rar_i; ///< rar/unrar executable and list options
			static QStringList ace_i; ///< unace executable and list options
			static QStringList targz_i;
			static QStringList tarbz2_i;
			static bool havezip;
			static bool haverar;
			static bool haveace;
			static bool havetargz;
			static bool havetarbz2;

			//! Determines archive type
			/*! Determines archive type basing on filename extension. As a last effort, if
			 *  the check fails, tries to guess archive type basing on well known magic bytes.
			 *  @param filename name of the archive, with path */
			static ArchiveType archiveType(const QString &filename);
			int extract(const QString &filename, const QString &destdir);
			void init();

		protected slots:
			void extractExited();
			void extractStdoutReady();
			void infoStdoutReady();
			void infoExited();

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
			static bool autoconfACE();
			static bool autoconfTARGZ();
			static bool autoconfTARBZ2();
			static bool haveRAR();
			static bool haveZIP();
			static bool haveACE();
			static bool haveTARGZ();
			static bool haveTARBZ2();
	};
}

#endif

