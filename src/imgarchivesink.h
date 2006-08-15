/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <yogin@linux.bydg.org>
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
#include <qvaluelist.h>
#include "imgdirsink.h"

class QImage;
class QProcess;

namespace QComicBook
{
	//! Comic book archive sink.
	/*! Allows opening different kind of archives containing image files. */
	class ImgArchiveSink: public ImgDirSink
	{
		Q_OBJECT

		public:
			enum ArchiveType {
				RAR_ARCHIVE = 1,
				ZIP_ARCHIVE = 2,
				ACE_ARCHIVE = 4,
				TARGZ_ARCHIVE = 8,
				TARBZ2_ARCHIVE = 16,
				UNKNOWN_ARCHIVE = 2<<31
			};

			struct ArchiveTypeInfo
			{
				ArchiveType type;
				QString name;
				QStringList listopts;
				QStringList extractopts;
				QStringList compressopts;
				QStringList extensions;
				bool reading;
				bool writing;
			};

		protected:

			ArchiveType archivetype; ///< the type of currently opened archive
			QProcess *pext; ///< extracting process
			QProcess *pinf; ///< file list extracing process
			QString archivename; ///< archive file name, without path
			QString archivepath; ///< full path, including archive name
			QString tmppath; ///< path to extracted archive
			QStringList archfiles; ///< list of archive files
			QStringList archdirs; ///< list of archive dirs
			int filesnum; ///< number of files gathered from parsing archiver output, used for progress bar
			int extcnt; ///< extracted files counter for progress bar

			static QValueList<ArchiveTypeInfo> archinfo;
			
			static int suppopen;
			static int suppsave;
			static QString openext; ///< supported archives extensions (for open)
			static QString saveext; ///< supported archives extensions (for save)

			static void autoconfRAR();
			static void autoconfZIP();
			static void autoconfACE();
			static void autoconfTARGZ();
			static void autoconfTARBZ2();

			//! Determines archive type
			/*! Determines archive type basing on filename extension. As a last effort, if
			 *  the check fails, tries to guess archive type basing on well known magic bytes.
			 *  @param filename name of the archive, with path */
			static ArchiveType archiveType(const QString &filename);
			int extract(const QString &filename, const QString &destdir);
			void init();
			virtual void doCleanup();
			
		protected slots:
			void extractExited();
			void extractStdoutReady();
			void infoStdoutReady();
			void infoExited();

		public:
			ImgArchiveSink();
			ImgArchiveSink(const QString &path);
			ImgArchiveSink(const ImgDirSink &sink);
			virtual ~ImgArchiveSink();

			virtual int open(const QString &path);
			virtual void close();
			virtual QString getName(int maxlen = 50);
			virtual QString getFullName() const;

			virtual bool supportsNext() const;
			virtual QString getNext() const;
			virtual QString getPrevious() const;

			static void autoconfArchivers();
			static int supportedArchives();
			static QValueList<ImgArchiveSink::ArchiveTypeInfo> supportedArchivesInfo();
			static bool supportsOpen(ArchiveType t);
			static bool supportsSave(ArchiveType t);
			static QString supportedOpenExtensions();
			static QString supportedSaveExtensions();
			static QString makeTempDir();
	};
}

#endif

