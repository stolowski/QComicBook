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
#include <QObject>
#include <QStringList>
#include <QDateTime>
#include <QMap>
#include <QMutex>
#include "DirReader.h"
#include "Page.h"

class QImage;

namespace QComicBook
{

	//! Possible errors.
	enum SinkError
	{
		SINKERR_ACCESS = 1, //!<can't access file or directory
		SINKERR_UNKNOWNFILE, //!<unknown archive format
		SINKERR_NOTSUPPORTED, //!<known archive type, but not supported
		SINKERR_LOADERROR,  //!<can't load image file
		SINKERR_NOTFOUND, //!<file not found
		SINKERR_NOTFILE,   //!<not a regular file
		SINKERR_NOTDIR,    //!<not a directory
		SINKERR_EMPTY,     //!<no images inside
		SINKERR_ARCHEXIT, //!<archiver exited with error
		SINKERR_OTHER  //!<another kind of error
	};

	class Thumbnail;
	class ImgCache;

	//! Comic book directory sink.
	/*! Allows opening directories containing image files. */
	class ImgDirSink: public QObject, protected DirReader
	{
		Q_OBJECT
		
		signals:
			//! Emited to report progress of comic book reading.
			/*! This signal may be used in conjunction with QProgressBar class.
			 *  @param current current progress value
			 *  @param total total number of steps */
			void progress(int current, int total);

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

			//! Scans directories recursively for image and text files.
			/*! Scans directories for jpg, png, gif, xpm image files and .nfo and file_id.diz text files.
			 *  Images file names are stored in imgfiles; text file names are stored in txtfiles.
			 *  Other files are store in otherfiles. All directories are stored in dirs.
			 *  @param s starting directory 
			 *  @see imgfiles
			 *  @see txtfiles
			 *  @see otherfiles
			 *  @see dirs */
			//void recurseDir(const QString &s);
			static QString memPrefix(int &s);
			void setComicBookName(const QString &name);

			static const int MAX_TEXTFILE_SIZE;
		
			virtual bool fileHandler(const QFileInfo &finfo);
		
		private:
			ImgCache *cache;
			mutable QMutex listmtx; //!< mutex for imgfiles
			QStringList imgfiles; //!< list of images files in directory
			QStringList txtfiles; //!< text files (.nfo, file_id.diz)
			QStringList otherfiles; //!< list of other files
			QStringList dirs; //!< directories
			QString dirpath; //!< path to directory
			QString cbname; //!< comic book name (directory path by default)
			QMap<QString, FileStatus> timestamps; //!< last modifications timestamps for all pages
			mutable QStringList desc; //txt files

		public:
			ImgDirSink(bool dirs=false, int cacheSize=0);
			ImgDirSink(const QString &path, bool dirs=false, int cacheSize=0);
			ImgDirSink(const ImgDirSink &sink, int cacheSize=0);
			virtual ~ImgDirSink();

			void setCacheSize(int cacheSize, bool autoAdjust);

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
			virtual Page getImage(unsigned int num, int &result);

			//! Returns thumbnail image for specified page.
			/*! Thumbnail is loaded from disk if found and caching is enabled. Otherwise,
			 *  the image is loaded and thumbnail is generated.
			 *  @param num page number
			 *  @param thumbcache specifies if thumbnails disk cache should be used */
			virtual Thumbnail getThumbnail(int num, bool thumbcache=true);

			/*! @return number of images for this comic book sink */
			virtual int numOfImages() const;
			
			/*! Returns abbreviated name of maxlen. This name is not reliable, eg. it can't
			 *  be used as file or directory name. It is meant for viewing only.
			 *  @param maxlen maximum length of the name that will be returned
			 *  @return abbreviated comic book name */
			virtual QString getName(int maxlen = 50);

			/*! Returns full name of the comic book.
			 *  @return name of comic book */
			virtual QString getFullName() const;
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

