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

/*! \file imgloader.h */

#ifndef __IMGLOADERTHREAD_H
#define __IMGLOADERTHREAD_H

#include <qthread.h>
#include <qvaluelist.h>

namespace QComicBook
{
	class ImgDirSink;

	//! Thread-based image loader.
	class ImgLoaderThread: public QThread
	{
		protected:
			volatile QThread::Priority prio; //!<thread priority
			QMutex mtx; //!<mutex for serialization of class attributes
			QValueList<int> requests; //!<the list of requested pages
			ImgDirSink *sink;

			volatile bool stopped;

			//! Main function of the thread.
			/*! Preloads requested pages from requests list using sink->getImage().
			 *  Stop if stopped flag is true.
			 *  @see ImgDirSink::getImage
			 */
			virtual void run();

		public:
			ImgLoaderThread();
			virtual ~ImgLoaderThread();

			//! Changes priority of the loader thread.
			/*! @param p new priority
			 */
			virtual void setPriority(QThread::Priority p);

			//! Sets image source sink.
			/*! @param sink image sink used for retrieving (loading) images
			 */
			virtual void setSink(ImgDirSink *sink=NULL);

			//! Appends page to the list of pages to load.
			/*! @param page page to load
			 */
			virtual void request(int page);

			//! Appends few pages to the list of pages to load.
			/*! @param first starting page
			 *  @param n number of pages to load in turn
			 */
			virtual void request(int first, int n);

			//! Stops processing requests and exits thread execution.
			virtual void stop();
	};
}

#endif

