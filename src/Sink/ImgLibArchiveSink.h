#ifndef __IMGLIBARCHIVESINK_H
#define __IMGLIBARCHIVESINK_H

#include "ImgSink.h"
#include <archive.h>

namespace QComicBook
{
	class DirEntry;

	//! Comic book archive sink based on libarchive.
	/*! Allows opening different kind of archives containing image files, without decompressing all files to disk. */
	class ImgLibArchiveSink: public ImgSink
	{
			public:
				ImgLibArchiveSink(int cacheSize=0);

				virtual ~ImgLibArchiveSink();

				virtual void setCacheSize(int cacheSize, bool autoAdjust);

				virtual int open(const QString &path);

				virtual void close();

				virtual QImage image(unsigned int num, int &result);

				virtual Thumbnail getThumbnail(unsigned int num, bool thumbcache=true);

				virtual int numOfImages() const;
				
				void setComicBookName(const QString &name, const QString &fullName);
				QString getName(int maxlen = 50) const;

				QString getFullName() const;

				virtual QString getFullFileName(int page) const;

				virtual QStringList getDescription() const;

				virtual bool timestampDiffers(int page) const;
				virtual bool hasModifiedFiles() const;

				virtual bool supportsNext() const;

				virtual QString getNext() const;

				virtual QString getPrevious() const;
			private:
				static QStringList splitPath(const QString &path);

				QString archivepath;
				DirEntry *root;
		};
}

#endif
