#ifndef __ARCHIVERWIN_H
#define __ARCHIVERWIN_H

#include <qdialog.h>
#include <qmap.h>
#include "imgarchivesink.h"

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
			QMap<int, ImgArchiveSink::ArchiveType> map; //<! maps cm_archtype indexes to archive types

		protected slots:
			void archiveTypeChanged(int index);

		public slots:
			void browse();

		public:
			ArchiverDialog(QWidget *parent, ImgDirSink *sink);
			~ArchiverDialog();
	};
}

#endif

