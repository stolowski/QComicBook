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

#ifndef __IMGARCHIVER
#define __IMGARCHIVER

/*! \file imgarchiver.h */

#include "ImgArchiveSink.h"
#include <QList>
#include <QProcess>

namespace QComicBook
{
	class ImgArchiver: public ImgArchiveSink
	{
		Q_OBJECT

		signals:
			void createProgress(int current, int total);
			void createReady();
			void createError(int code);
			
		protected:
			QProcess *pomp; ///< archiving process
			QString tmpcreatpath;
			QStringList newfiles; ///< list of new archive files
			int newfilesnum; ///< newfiles count
			int cmpcnt; ///< archiver output counter

			virtual void doCleanup();

		protected slots:
			void compressExited(int exitCode, QProcess::ExitStatus status);
			void compressStdoutReady();

		public:
			ImgArchiver(const ImgDirSink &sink);
			virtual ~ImgArchiver();
			
			virtual void create(const QString &destname, ArchiveType type, QList<int> pages);
	};
}

#endif

