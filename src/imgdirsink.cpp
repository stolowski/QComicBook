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

#include "imlibimage.h"
#include "imgdirsink.h"
#include "cbsettings.h"
#include "thumbnail.h"
#include <qimage.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>

//
// maximum size of description file (won't load files larger than that)
#define MAX_TEXTFILE_SIZE 65535

using namespace QComicBook;
                        
const QString ImgDirSink::imgext[] = {".jpg", ".jpeg", ".png", ".gif", ".xpm", NULL};

ImgDirSink::ImgDirSink(bool dirs): QObject(), dirpath(QString::null), dirsfirst(dirs)
{
        thloader.setSink(this);
        imgloader.setSink(this);
}

ImgDirSink::ImgDirSink(const QString &path, bool dirs): QObject(), dirpath(QString::null), dirsfirst(dirs)
{
        thloader.setSink(this);
        imgloader.setSink(this);
        open(path);
}

ImgDirSink::ImgDirSink(const ImgDirSink &sink): QObject()
{
        thloader.setSink(this);
        imgloader.setSink(this);

	dirpath = sink.dirpath;
	cbname = sink.cbname;
	imgfiles = sink.imgfiles;
	txtfiles = sink.txtfiles;
	otherfiles = sink.otherfiles;
	dirs = sink.dirs;
	timestamps = sink.timestamps;
	dirsfirst = sink.dirsfirst;
}

ImgDirSink::~ImgDirSink()
{
        close();
}

void ImgDirSink::setComicBookName(const QString &name)
{
        cbname = name;
}

QString ImgDirSink::memPrefix(int &s) const
{
        QString mempfix;
        if (s < 1024)
        {
                mempfix = tr("bytes");
        }
        else if (s < 1024*1024)
        {
                s /= 1024;
                mempfix = tr("Kbytes");
        }
        else
        {
                s /= 1024*1024;
                mempfix = tr("Mbytes");
        }
        return mempfix;
}

void ImgDirSink::recurseDir(const QString &s)
{
        QDir dir(s);
        dir.setSorting(QDir::DirsFirst|QDir::Name);
        dir.setMatchAllDirs(true);
        const QStringList files = dir.entryList();
        for (QStringList::const_iterator it = files.begin(); it!=files.end(); ++it)
        {
                if (*it == "." || *it == "..")
                        continue;

                QFileInfo finf(dir.absFilePath(*it, false));
                if (finf.isFile())
                {
			if (knownImageExtension(*it))
                                imgfiles.append(finf.absFilePath());
                        else if ((*it).endsWith(".nfo", false) || (*it) == "file_id.diz")
                                txtfiles.append(finf.absFilePath());
                        else
                                otherfiles.append(finf.absFilePath());
			timestamps.insert(finf.absFilePath(), FileStatus(finf.lastModified()));
                }
                else if (finf.isDir() && (finf.absFilePath()!=s))
                {
                        dirs.append(finf.absFilePath());
                        recurseDir(finf.absFilePath());
                }
        }
}

int ImgDirSink::open(const QString &path)
{
        int status;
        emit progress(0, 1);
        QFileInfo info(path);
        if (!info.exists())
                status = SINKERR_NOTFOUND;
        else
        {
                if (info.isDir())
                {
                        if (info.isReadable() && info.isExecutable())
                        {
                                dirpath = path;
                                recurseDir(path);
                                status = (numOfImages() > 0) ? 0 : SINKERR_EMPTY;
                        }
                        else
                                status = SINKERR_ACCESS;
                }
                else status = SINKERR_NOTDIR;
        }
        setComicBookName(path);
        if (status == 0)
        {
                emit progress(1, 1);
                emit sinkReady(path);
        }
        else
                emit sinkError(status);
        return status;
}

void ImgDirSink::close()
{
        thloader.stop();
        imgloader.stop();
        thloader.wait(); //wait for thumbnail loader thread
        imgloader.wait(); //wait for preload thread

        listmtx.lock();
        dirpath = QString::null;
        imgfiles.clear();
        txtfiles.clear();
        otherfiles.clear();
        dirs.clear();
        listmtx.unlock();
}

QString ImgDirSink::getName(int maxlen)
{
        if (dirpath.length() < maxlen)
                return dirpath;
        QString tmpname = "..." + dirpath.right(maxlen-3);
        return tmpname;
}

QString ImgDirSink::getFullName()
{
        return dirpath;
}

QString ImgDirSink::getFullFileName(int page) const
{
	return page < numOfImages() ? imgfiles[page] : QString::null;
}

QStringList ImgDirSink::getDescription() const
{
        QStringList desc;
        
        for (QStringList::const_iterator it = txtfiles.begin(); it!=txtfiles.end(); it++)
        {
                QFileInfo finfo(*it);
                QFile f(*it);
                if (f.open(IO_ReadOnly) && (f.size() < MAX_TEXTFILE_SIZE))
                {
                        QString cont;
                        QTextStream str(&f);
                        while (!str.atEnd())
                                cont += str.readLine() + "\n";
                        f.close();
                        desc.append(finfo.fileName()); //append file name
                        desc.append(cont); //and contents
                }
        }
        return desc;
}

QString ImgDirSink::getStats() const
{
        int w, h, p;
        int cmem = 0;
        
        //
        // calculate occupied memory and dimensions of the largest image
        p = w = h = 0;
        /*for (QCacheIterator<QImage> it(*cache); it.current(); ++it)
        {
                QImage *img = it.current();
                int tmp = img->width() * img->height();
                if (tmp > p)
                {
                        p = tmp;
                        w = img->width();
                        h = img->height();
                }
                cmem += it.current()->numBytes();
        }*/

        int amem = 0;
        const QString cmempfix = memPrefix(cmem);
        const QString amempfix = memPrefix(amem);
        QString info;

        QTextStream str(&info, IO_WriteOnly);
        str << tr("Number of pages") << ": " << numOfImages() << endl << 
               tr("Largest cached image") << ": " << w << "x" << h << endl;
        return info;
}

ImlibImage* ImgDirSink::getImage(unsigned int num, int &result, int preload)
{
        result = SINKERR_LOADERROR;

        listmtx.lock();
        const int imgcnt = imgfiles.count();
        if (num < imgcnt)
	{
		const QString fname = imgfiles[num];
		listmtx.unlock();

		ImlibImage *im = new ImlibImage();
		if (!im->load(fname))
		{
			delete im;
			im = NULL;
		}
		result = 0;

		/*const QFileInfo finf(fname);

		if (rimg.load(fname))
		{
			result = 0;
			if (timestamps[fname] != finf.lastModified())
				timestamps[fname].set(finf.lastModified(), true);
		}*/
		return im;
	}
        else
                listmtx.unlock();
	return NULL;
}

Thumbnail* ImgDirSink::getThumbnail(int num, bool thumbcache)
{
        QString fname;
        listmtx.lock();
        if (num < imgfiles.count())
                fname = imgfiles[num];
        else
        {
                listmtx.unlock();
                return NULL;
        }
        listmtx.unlock();

        Thumbnail *t = new Thumbnail(num);
        QString thname;

        //
        // try to load cached thumbnail
        if (thumbcache)
        {
                thname = ComicBookSettings::thumbnailsDir() + "/" + cbname.remove('/') + QString::number(num) + ".jpg";
                if (t->tryLoad(thname))
                {
                        t->touch(thname);
                        return t;
                }
        }
        
        //
        // try to load image
        if (!t->tryLoad(fname))
        {
                delete t;
                return NULL;
        }

        //
        // save thumbnail if caching enabled
        if (thumbcache)
                t->save(thname);
        return t;
}

void ImgDirSink::requestThumbnail(int num)
{
        thloader.request(num);
}

void ImgDirSink::requestThumbnails(int first, int n)
{
        thloader.request(first, n);
}

int ImgDirSink::numOfImages() const
{
        listmtx.lock();
        const int n = imgfiles.count();
        listmtx.unlock();
        return n;
}

QStringList ImgDirSink::getAllfiles() const
{
        listmtx.lock();
        QStringList l = imgfiles + txtfiles + otherfiles;
        listmtx.unlock();
        return l;
}

QStringList ImgDirSink::getAlldirs() const
{
        return dirs;
}

QStringList ImgDirSink::getAllimgfiles() const
{
        listmtx.lock();
        const QStringList l = imgfiles;
        listmtx.unlock();
        return l;
}

bool ImgDirSink::timestampDiffers(int page) const
{
	if (page < 0 || page > numOfImages())
		return false;
	listmtx.lock();
	const QString fname = imgfiles[page];
	listmtx.unlock();
	QFileInfo f(fname);
	return f.lastModified() != timestamps[fname];
}
			
bool ImgDirSink::hasModifiedFiles() const
{
	//
	// check timestamps of all files
	for (QMap<QString, FileStatus>::ConstIterator it = timestamps.begin(); it != timestamps.end(); ++it)
	{
		QFileInfo finf(it.key());
		if (it.data().isModified() || it.data() != finf.lastModified())
			return true;
	}
	return false;
}

void ImgDirSink::removeThumbnails(int days)
{       
        if (days < 1)
                return;

        const QDateTime currdate = QDateTime::currentDateTime();
        
        QDir dir(ComicBookSettings::thumbnailsDir(), "*.jpg", QDir::Unsorted, QDir::Files|QDir::NoSymLinks);
        const QStringList files = dir.entryList();
        for (QStringList::const_iterator it = files.begin(); it!=files.end(); it++)
        {
                QFileInfo finfo(dir.absFilePath(*it));
                if (finfo.lastModified().daysTo(currdate) > days)
                        dir.remove(*it);
        }
}

ThumbnailLoaderThread& ImgDirSink::thumbnailLoader()
{
        return thloader;
}

bool ImgDirSink::knownImageExtension(const QString &path)
{
	for (int i=0; imgext[i]; i++)
		if (path.endsWith(imgext[i], false))
			return true;
	return false;
}

QString ImgDirSink::getKnownImageExtension(const QString &path)
{
	for (int i=0; imgext[i]; i++)
		if (path.endsWith(imgext[i], false))
			return imgext[i];
	return false;
}

