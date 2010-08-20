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

#ifndef __FRAME_DETECT_THREAD_H
#define __FRAME_DETECT_THREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QList>
#include <ComicFrameList.h>

namespace QComicBook
{
	class Page;

	class FrameDetectThread: public QThread
	{
		Q_OBJECT

		public:
			FrameDetectThread();
			~FrameDetectThread();

			virtual void run();
			void stop();
			void clear();

		public slots:
			void process(const Page &p);

		signals:
			void framesReady(const ComicFrameList &frames);

		private:
			volatile bool m_stop;
			QList<Page> m_pages;
			QMutex m_processListMtx;
			QMutex m_condMtx;
			QWaitCondition m_reqCond;
	};
}

#endif
