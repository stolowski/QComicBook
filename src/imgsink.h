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

#ifndef __IMGSINK_H
#define __IMGSINK_H

#include <qstring.h>
#include <qobject.h>

class QImage;
class Thumbnail;

class ImgSink: public QObject
{
	Q_OBJECT

	signals:
		void sinkReady(const QString &path);
		void sinkError(int code);
		void progress(int current, int total);

	public slots:
		virtual void setThumbnailReciever(QObject *rcv) = 0;
		virtual void requestThumbnail(int num) = 0;
		virtual void requestThumbnails(int first, int n) = 0;

	public:
		ImgSink();
		virtual ~ImgSink();

		virtual int open(const QString &path) = 0;
		virtual void close() = 0;
		virtual QImage getImage(unsigned int num, int &result, int preload=0) = 0;
		virtual Thumbnail* getThumbnail(int num, bool thumcache=true) = 0;
		virtual int numOfImages() const = 0;

		//
		// Returns abbreviated name of maxlen. This name is not reliable, eg. it can't
		// be used as file or directory name. It is meant for viewing only
		virtual QString getName(int maxlen = 50) = 0;

		//
		// Returns full name
		virtual QString getFullName() = 0;

		//
		// Returns contents of .nfo and file_id.diz files; file name goes first, then contents
		virtual QStringList getDescription() const = 0;

		//
		// Returns statistics regarding number of images, memory usage etc.
		virtual QString getStats() const = 0;
};

#endif

