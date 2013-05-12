/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __IMGSINK_H
#define __IMGSINK_H

#include <QObject>

class QImage;

namespace QComicBook
{
	class Page;
	class Thumbnail;
	class ImgCache;
        class PageSorter;

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
		
	class ImgSink: public QObject
	{
		Q_OBJECT

		signals:
			//! Emited to report progress of comic book reading.
			/*! This signal may be used in conjunction with QProgressBar class.
			 *  @param current current progress value
			 *  @param total total number of steps */
			void progress(int current, int total);

		public:
			ImgSink(int cacheSize=0);

			virtual ~ImgSink();

			virtual void setCacheSize(int cacheSize, bool autoAdjust);

			//! Opens this comic book sink with specified path.
			/*! @param path comic book location
			 *  @return value grater than 0 for error; 0 on success */
			virtual int open(const QString &path) = 0;

                        virtual void sort(const PageSorter &sorter) = 0;

			//! Closes this comic book sink cleaning resources.
			virtual void close() = 0;


			//! Returns given page. Has to be implemented by subclass.
			/*!
			*/
			virtual QImage image(unsigned int num, int &result) = 0;

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
			virtual Thumbnail getThumbnail(unsigned int num, bool thumbcache=true);

			/*! @return number of images for this comic book sink */
			virtual int numOfImages() const = 0;
			
			void setComicBookName(const QString &name, const QString &fullName);

			/*! Returns abbreviated name of maxlen. This name is not reliable, eg. it can't
			 *  be used as file or directory name. It is meant for viewing only.
			 *  @param maxlen maximum length of the name that will be returned
			 *  @return abbreviated comic book name */
			QString getName(int maxlen = 50) const;

			/*! Returns full name of the comic book.
			 *  @return name of comic book */
			QString getFullName() const;

			virtual QString getFullFileName(int page) const = 0;

			/*! @return contents of .nfo and file_id.diz files; file name goes first, then contents. */
			virtual QStringList getDescription() const = 0;

			virtual bool supportsNext() const = 0;

			//! Returns the next filename to open.
			/* @return next filename or QString::null */
			virtual QString getNext() const = 0;

			virtual QString getPrevious() const = 0;

		private:
			ImgCache *cache;
			QString cbname; //!< comic book name
			QString cbfullname; //!< full comic book name (e.g. path)
	};
}

#endif
