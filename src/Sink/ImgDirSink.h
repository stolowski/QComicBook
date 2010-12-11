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

/*! \file imgdirsink.h */

#ifndef __IMGDIRSINK_H
#define __IMGDIRSINK_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QMap>
#include <QMutex>
#include "DirReader.h"
#include "ImgSink.h"
#include "Page.h"

class QImage;

namespace QComicBook
{

	class Thumbnail;

	//! Comic book directory sink.
	/*! Allows opening directories containing image files. */
	class ImgDirSink: public ImgSink, protected DirReader
	{
		protected:
			class FileStatus
			{
				private:
					bool modified;
					QDateTime timestamp;
				public:
					FileStatus(): modified(false) {}
					FileStatus(const QDateTime &mtime): modified(false), timestamp(mtime) {}
					void set(const QDateTime &mtime, bool m) { timestamp = mtime; modified = m; }
					bool isModified() const { return modified; }
					bool operator==(const QDateTime &d) const { return d == timestamp; }
					bool operator!=(const QDateTime &d) const { return d != timestamp; }
					operator QDateTime() const { return timestamp; }
			};

			static QString memPrefix(int &s);

			static const int MAX_TEXTFILE_SIZE;
		
			virtual bool fileHandler(const QFileInfo &finfo);
		
		private:
			mutable QMutex listmtx; //!< mutex for imgfiles
			QStringList imgfiles; //!< list of images files in directory
			QStringList txtfiles; //!< text files (.nfo, file_id.diz)
			QStringList otherfiles; //!< list of other files
			QStringList dirs; //!< directories
			QString dirpath; //!< path to directory
			QMap<QString, FileStatus> timestamps; //!< last modifications timestamps for all pages
			mutable QStringList desc; //txt files

		public:
			ImgDirSink(bool dirs=false, int cacheSize=0);
			ImgDirSink(const QString &path, bool dirs=false, int cacheSize=0);
			ImgDirSink(const ImgDirSink &sink, int cacheSize=0);
			virtual ~ImgDirSink();

			//! Opens this comic book sink with specifiled path.
			/*! @param path comic book location
			 *  @return value grater than 0 for error; 0 on success */
			virtual int open(const QString &path);


			//! Closes this comic book sink cleaning resources.
			virtual void close();

			//! Returns an image for specified page.
			/*! The cache is first checked for image. If not found, the image is loaded.
			 *  @param num page number
			 *  @param result contains 0 on succes or value greater than 0 for error
			 *  @return an image */
			virtual QImage image(unsigned int num, int &result);

			/*! @return number of images for this comic book sink */
			virtual int numOfImages() const;
			
			virtual QString getFullFileName(int page) const;

			/*! @return contents of .nfo and file_id.diz files; file name goes first, then contents. */
			virtual QStringList getDescription() const;

			virtual QStringList getAllfiles() const;
			virtual QStringList getAlldirs() const;
			virtual QStringList getAllimgfiles() const;
			virtual bool timestampDiffers(int page) const;
			virtual bool hasModifiedFiles() const;

			virtual bool supportsNext() const;

			//! Returns the next filename to open.
			/* @return next filename or QString::null */
			virtual QString getNext() const;

			virtual QString getPrevious() const;

			//
			//! Removes old thumbnails.
			/*! @param days thumbnails older than this number will be removed */
			static void removeThumbnails(int days);

			static bool knownImageExtension(const QString &path);
			static QString getKnownImageExtension(const QString &path);
			static QStringList getKnownImageExtensionsList();
	};
}

#endif

