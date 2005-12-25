#ifndef __ARCHIVERWIN_H
#define __ARCHIVERWIN_H

#include <qdialog.h>

/*! \file archiverwin.h */

class QLineEdit;
class QComboBox;

namespace QComicBook
{
	class ImgDirSink;

	class ArchiverDialog: public QDialog
	{
		Q_OBJECT

		private:
			QLineEdit *le_destname;
			QComboBox *cm_archtype;
			ImgDirSink *imgsink;

		public slots:
			void browse();

		public:
			ArchiverDialog(QWidget *parent, ImgDirSink *sink);
			~ArchiverDialog();
	};
}

#endif

