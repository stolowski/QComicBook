#ifndef __LIBRARY_WINDOW_H
#define __LIBRARY_WINDOW_H

#include "ui_LibraryWindow.h"
#include <QWidget>

namespace QComicBook
{
	class LibraryWindow: public QWidget, private Ui::LibraryWindow
	{
		Q_OBJECT

		public:
			LibraryWindow(QWidget *parent);
			virtual ~LibraryWindow();
	};
}

#endif
