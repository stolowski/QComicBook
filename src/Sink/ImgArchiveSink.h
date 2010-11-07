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
#include "ImgDirSink.h"

class QImage;

namespace QComicBook
{
	//! Comic book archive sink.
	/*! Allows opening different kind of archives containing image files. */
	class ImgArchiveSink: public ImgDirSink
	{
		Q_OBJECT

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

                        static int waitForFinished(QProcess *p);
                        int extract(const QString &filename, const QString &destdir, QStringList extargs, QStringList infargs);
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

			static QString makeTempDir(const QString &parent = QDir::tempPath());
	};
}

#endif

