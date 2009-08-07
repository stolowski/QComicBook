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

/*! \file imgarchivesink.h */

#ifndef __IMGARCHIVESINK_H
#define __IMGARCHIVESINK_H

#include <QString>
#include <QObject>
#include <QStringList>
#include <QList>
#include <QProcess>
#include "imgdirsink.h"

class QImage;

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
				SEVENZIP_ARCHIVE = 32,
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
			QProcess *pext; ///< extracting process
			QProcess *pinf; ///< file list extracing process
			QString archivename; ///< archive file name, without path
			QString archivepath; ///< full path, including archive name
			QString tmppath; ///< path to extracted archive
			QStringList archfiles; ///< list of archive files
			QStringList archdirs; ///< list of archive dirs
			int filesnum; ///< number of files gathered from parsing archiver output, used for progress bar
			int extcnt; ///< extracted files counter for progress bar

			static QList<ArchiveTypeInfo> archinfo;
			
			static int suppopen;
			static int suppsave;
			static QStringList openext; ///< supported archives extensions (for open)
			static QStringList saveext; ///< supported archives extensions (for save)

			static void autoconfRAR();
			static void autoconfZIP();
			static void autoconfACE();
			static void autoconfTARGZ();
			static void autoconfTARBZ2();
			static void autoconfSEVENZIP();

			//! Determines archive type
			/*! Determines archive type basing on filename extension. As a last effort, if
			 *  the check fails, tries to guess archive type basing on well known magic bytes.
			 *  @param filename name of the archive, with path */
			static ArchiveType getArchiveType(const QString &filename);
                        static int waitForFinished(QProcess *p);
			int extract(const QString &filename, const QString &destdir, ArchiveType archiveType=ImgArchiveSink::UNKNOWN_ARCHIVE);
			void init();
			virtual void doCleanup();
			
			virtual bool fileHandler(const QFileInfo &finfo);

		protected slots:
			void extractExited(int code, QProcess::ExitStatus exitStatus);
			void extractStdoutReady();
			void infoStdoutReady();
			void infoExited(int code, QProcess::ExitStatus exitStatus);

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
			static QList<ImgArchiveSink::ArchiveTypeInfo> supportedArchivesInfo();
			static bool supportsOpen(ArchiveType t);
			static bool supportsSave(ArchiveType t);
			static QStringList supportedOpenExtensions();
			static QStringList supportedSaveExtensions();
			static QString makeTempDir(const QString &parent = QDir::tempPath());
	};
}

#endif

