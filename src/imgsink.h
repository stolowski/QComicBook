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

/*! \file imgsink.h */

#ifndef __IMGSINK_H
#define __IMGSINK_H

#include <qstring.h>
#include <qobject.h>

class QImage;
class Thumbnail;

//! An abstract comic book image sink class.
/*! Provides one common interface for accessing comic book resources. */
class ImgSink: public QObject
{
	Q_OBJECT

	signals:
	        //! Emited when comic book is ready for reading.
		/*! @param path path of the opened comic book */
		void sinkReady(const QString &path);

		//! Emited on error.
		/*! @param code error number, as defined in SinkError.
		 *  @see SinkError */
		void sinkError(int code);

		//! Emited to report progress of comic book reading.
		/*! This signal may be used in conjunction with QProgressBar class.
		 *  param current current progress value
		 *  total total number of steps */
		void progress(int current, int total);

	public slots:
		//! Sets the thumbnails reciever object.
		/*! Sets the reciever of thumbnails for thumbnails loader thread. The thumbnail images
		 *  will be passed to this object as custom events using QCustomEvent class.
		 *  @param rcv reciever object */
		virtual void setThumbnailReciever(QObject *rcv) = 0;

		//! Requestes thumbnail to be loaded in separate thread.
		/*! @param num page number */
		virtual void requestThumbnail(int num) = 0;

		//! Requests number of thumbnails to be loaded in separate thread.
		/*! @param first first page number
		 *  @page n number of thumbnails to load */
		virtual void requestThumbnails(int first, int n) = 0;

	public:
		ImgSink();
		virtual ~ImgSink();

		//! Opens this comic book sink with specifiled path.
		/*! @param path comic book location
		 *  @return value grater than 0 for error; 0 on success */
		virtual int open(const QString &path) = 0;

		//! Closes this comic book sink cleaning resources.
		virtual void close() = 0;

		//! Returns an image for specifiled page.
		/*! The imgcache is first checked for image. If not found, the image is loaded. Optionally,
		 *  the preload thread is started to preload next images.
		 *  @param num page number
		 *  @param result contains 0 on succes or value greater than 0 for error
		 *  @param preload number of pages to preload in preloading thread 
		 *  @return an image */
		virtual QImage getImage(unsigned int num, int &result, int preload=0) = 0;

		//! Returns thumbnail image for specified page.
		/*! Thumbnail is loaded from disk if found and caching is enabled. Otherwise,
		 *  the image is loaded and thumbnail is generated.
		 *  @param num page number
		 *  @param thumcache specifies if thumbnails disk cache should be used */
		virtual Thumbnail* getThumbnail(int num, bool thumcache=true) = 0;

		/*! @return number of images for this comic book sink */
		virtual int numOfImages() const = 0;

		/*! Returns abbreviated name of maxlen. This name is not reliable, eg. it can't
		 *  be used as file or directory name. It is meant for viewing only.
		 *  @param maxlen maximum length of the name that will be returned
		 *  @return abbreviated comic book name */
		virtual QString getName(int maxlen = 50) = 0;

		/*! Returns full name of the comic book.
		 *  @return name of comic book */
		virtual QString getFullName() = 0;

		/*! @return contents of .nfo and file_id.diz files; file name goes first, then contents. */
		virtual QStringList getDescription() const = 0;

		/*! @return statistics regarding number of images, memory usage etc. */
		virtual QString getStats() const = 0;
};

#endif

