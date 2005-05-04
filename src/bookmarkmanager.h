#ifndef __BOOKMARKMANAGER_H
#define __BOOKMARKMANAGER_H

#include <qdialog.h>

class Bookmarks;

class BookmarkManager: public QDialog
{
	public:
		BookmarkManager(QWidget *parent, Bookmarks *b);
		~BookmarkManager();
};

#endif

