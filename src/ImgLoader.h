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

/*! \file imgloader.h */

#ifndef __IMGLOADERTHREAD_H
#define __IMGLOADERTHREAD_H

#include <QThread>
#include <QList>
#include <QMutex>
#include <QWaitCondition>

namespace QComicBook
{
	class ImgDirSink;
        class Page;

        struct LoadRequest
        {
            int pageNumber;
            bool twoPages;

            LoadRequest(int page, bool twoPages): pageNumber(page), twoPages(twoPages) {}
            bool operator==(const LoadRequest &r)
            {
                return pageNumber == r.pageNumber && twoPages == r.twoPages;
            }
        };

	//! Thread-based image loader.
	class ImgLoaderThread: public QThread
	{
            Q_OBJECT

		protected:
			volatile QThread::Priority prio; //!<thread priority
			QList<LoadRequest> requests; //!<the list of requested pages
			ImgDirSink *sink;
			QMutex loaderMutex;
                        QMutex condMutex;
                        QMutex sinkMutex;
                        QWaitCondition reqCond;
			volatile bool stopped;

			//! Main function of the thread.
			/*! Preloads requested pages from requests list using sink->getImage().
			 *  Stop if stopped flag is true.
			 *  @see ImgDirSink::getImage
			 */
			virtual void run();

                signals:
                        void pageLoaded(const Page &);
                        void pageLoaded(const Page &, const Page &);

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

			//! Stops processing requests and exits thread execution.
			virtual void stop();


               public slots:
			//! Appends page to the list of pages to load.
			/*! @param page page to load
			 */
			virtual void request(int page);

                        virtual void requestTwoPages(int page);

			//! Appends few pages to the list of pages to load.
			/*! @param first starting page
			 *  @param n number of pages to load in turn
			 */
                        virtual void request(int first, int n);

                        virtual void cancel(int page);
                        virtual void cancelTwoPages(int page);
                        virtual void cancelAll();
	};
}

#endif

