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

/*! \file imgloader.h */

#ifndef __IMGLOADERTHREAD_H
#define __IMGLOADERTHREAD_H

#include <qthread.h>
#include <qvaluelist.h>

class ImgSink;

//! Thread-based image loader.
class ImgLoaderThread: public QThread
{
	protected:
		volatile QThread::Priority prio; //!<thread priority
		QMutex mtx;
		QValueList<int> requests; //!<the list of requested pages
		ImgSink *sink;
				
		volatile bool stopped;
		virtual void run();
		
	public:
		ImgLoaderThread();
		virtual ~ImgLoaderThread();

		virtual void setPriority(QThread::Priority p);
		virtual void setSink(ImgSink *sink=NULL);
		virtual void request(int page);
		virtual void request(int first, int n);
		virtual void stop();
};

#endif

