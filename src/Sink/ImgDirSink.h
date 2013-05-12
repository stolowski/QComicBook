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
#include <QList>
#include <QDateTime>
#include <QMap>
#include <QMutex>
#include "DirReader.h"
#include "ImgSink.h"
#include "../Page.h"

class QImage;

namespace QComicBook
{

	class Thumbnail;
        class FileEntry;

	//! Comic book directory sink.
	/*! Allows opening directories containing image files. */
	class ImgDirSink: public ImgSink, protected DirReader
	{
		Q_OBJECT

		protected:
			static QString memPrefix(int &s);

			static const int MAX_TEXTFILE_SIZE;
		
			virtual bool fileHandler(const QFileInfo &finfo);
		
		private:
			mutable QMutex listmtx; //!< mutex for imgfiles
			QList<FileEntry> imgfiles; //!< list of images files in directory
			QList<FileEntry> txtfiles; //!< text files (.nfo, file_id.diz)
			QList<FileEntry> otherfiles; //!< list of other files
			QStringList dirs; //!< directories
			QString dirpath; //!< path to directory
			mutable QStringList desc; //txt files

		public:
			ImgDirSink(int cacheSize=0);
			ImgDirSink(const QString &path, int cacheSize=0);
			ImgDirSink(const ImgDirSink &sink, int cacheSize=0);
			virtual ~ImgDirSink();

			//! Opens this comic book sink with specifiled path.
			/*! @param path comic book location
			 *  @return value grater than 0 for error; 0 on success */
			virtual int open(const QString &path) override;

                        virtual void sort(const PageSorter &sorter) override;

			//! Closes this comic book sink cleaning resources.
			virtual void close() override;

			//! Returns an image for specified page.
			/*! The cache is first checked for image. If not found, the image is loaded.
			 *  @param num page number
			 *  @param result contains 0 on succes or value greater than 0 for error
			 *  @return an image */
			virtual QImage image(unsigned int num, int &result) override;

			/*! @return number of images for this comic book sink */
			virtual int numOfImages() const override;

			/*! @return contents of .nfo and file_id.diz files; file name goes first, then contents. */
			virtual QStringList getDescription() const override;

			virtual bool supportsNext() const override;

			//! Returns the next filename to open.
			/* @return next filename or QString::null */
			virtual QString getNext() const override;

			virtual QString getPrevious() const override;

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

