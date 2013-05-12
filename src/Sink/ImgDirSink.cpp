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

#include "ImgDirSink.h"
#include "ComicBookSettings.h"
#include "ImageFormatsInfo.h"
#include <QImage>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <algorithm>
#include "PageSorter.h"
#include "FileEntry.h"

using namespace QComicBook;

//
// maximum size of description file (won't load files larger than that)
const int ImgDirSink::MAX_TEXTFILE_SIZE = 65535;
                        
ImgDirSink::ImgDirSink(int cacheSize): ImgSink(cacheSize), dirpath(QString::null), DirReader(QDir::DirsLast|QDir::Name|QDir::IgnoreCase, 6)
{
}

ImgDirSink::ImgDirSink(const QString &path, int cacheSize): ImgSink(cacheSize), dirpath(QString::null), DirReader(QDir::DirsLast|QDir::Name|QDir::IgnoreCase, 6)
{
	open(path);
}

ImgDirSink::ImgDirSink(const ImgDirSink &sink, int cacheSize): ImgSink(cacheSize), DirReader(QDir::DirsLast|QDir::Name|QDir::IgnoreCase, 6)
{
	dirpath = sink.dirpath;
	imgfiles = sink.imgfiles;
	txtfiles = sink.txtfiles;
	otherfiles = sink.otherfiles;
	dirs = sink.dirs;
}

ImgDirSink::~ImgDirSink()
{
        close();
}

QString ImgDirSink::memPrefix(int &s)
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

bool ImgDirSink::fileHandler(const QFileInfo &finfo)
{
    FileEntry fe(dirpath, finfo);

    const QString fname = finfo.fileName();
    if (knownImageExtension(fname))
    {
        imgfiles.append(fe);
        return true;
    }
    if (fname.endsWith(".nfo", Qt::CaseInsensitive) || fname == "file_id.diz")
    {
        txtfiles.append(fe);
        return true;
    }
    otherfiles.append(fe);
    return false;
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
				visit(path);
                                status = (numOfImages() > 0) ? 0 : SINKERR_EMPTY;
                        }
                        else
                                status = SINKERR_ACCESS;
                }
                else status = SINKERR_NOTDIR;
        }
        setComicBookName(path, dirpath);
        if (status == 0)
                emit progress(1, 1);
        return status;
}

void ImgDirSink::sort(const PageSorter &sorter)
{
    std::sort(imgfiles.begin(), imgfiles.end(), sorter);
}

void ImgDirSink::close()
{
        listmtx.lock();
        dirpath = QString::null;
        imgfiles.clear();
        txtfiles.clear();
        otherfiles.clear();
        dirs.clear();
        listmtx.unlock();
}

QString ImgDirSink::getFullFileName(int page) const
{
    return page < numOfImages() ? imgfiles[page].getFullFileName () : QString::null;
}

QStringList ImgDirSink::getDescription() const
{
	if (desc.count() == 0) //read files only once
	{
            for (auto it = txtfiles.begin(); it!=txtfiles.end(); it++)
	    {
                QFileInfo finfo((*it).getFullFileName());
                QFile f((*it).getFullFileName());
                if (f.open(QIODevice::ReadOnly) && (f.size() < MAX_TEXTFILE_SIZE))
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
	}
        return desc;
}

QImage ImgDirSink::image(unsigned int num, int &result)
{
	result = SINKERR_LOADERROR;

	listmtx.lock();
	const int imgcnt = imgfiles.count();
	QImage im;

	if (num < imgcnt)
	{
            const QString fname = imgfiles[num].getFullFileName();
            listmtx.unlock();
            
            if (!im.load(fname))
                result = 1;
            else
                result = 0;

	}
	else
        {
            listmtx.unlock();
            result = 0;
        }

	const Page page(num, im);
	return page;
}

int ImgDirSink::numOfImages() const
{
        listmtx.lock();
        const int n = imgfiles.count();
        listmtx.unlock();
        return n;
}

bool ImgDirSink::supportsNext() const
{
	return false;
}

QString ImgDirSink::getNext() const
{
	return QString::null;
}

QString ImgDirSink::getPrevious() const
{
	return QString::null;
}

void ImgDirSink::removeThumbnails(int days)
{       
        if (days < 1)
                return;

        const QDateTime currdate = QDateTime::currentDateTime();
        
        QDir dir(ComicBookSettings::instance().thumbnailsDir(), "*.jpg", QDir::Unsorted, QDir::Files|QDir::NoSymLinks);
        const QStringList files = dir.entryList();
        for (auto it = files.begin(); it!=files.end(); it++)
        {
                QFileInfo finfo(dir.absoluteFilePath(*it));
                if (finfo.lastModified().daysTo(currdate) > days)
                        dir.remove(*it);
        }
}

bool ImgDirSink::knownImageExtension(const QString &path)
{
    foreach (QString ext, ImageFormatsInfo::instance().extensions())
    {
        if (path.endsWith(ext, Qt::CaseInsensitive))
        {
            return true;
        }
    }
    return false;
}

QString ImgDirSink::getKnownImageExtension(const QString &path)
{
    foreach (QString ext, ImageFormatsInfo::instance().extensions())
    {
        if (path.endsWith(ext, Qt::CaseInsensitive))
            return ext;
    }
    return QString::null;
}

QStringList ImgDirSink::getKnownImageExtensionsList()
{
    QStringList list;
    foreach (QString ext, ImageFormatsInfo::instance().extensions())
    {
        QString p("*");
        p.append(ext);
        list << p;
    }
    return list;
}

