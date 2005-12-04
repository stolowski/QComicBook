#include "archiverdialog.h"
#include "imgdirsink.h"
#include <qlayout.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qframe.h>
#include <qcombobox.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qsizepolicy.h>
#include <klocale.h>

using namespace QComicBook;

ArchiverDialog::ArchiverDialog(QWidget *parent, ImgDirSink *sink): QDialog(parent), imgsink(sink)
{
	QVBoxLayout *lay0 = new QVBoxLayout(this, 5, 5);
	
	QVBox *box0 = new QVBox(this);	
	box0->setMargin(5);
	box0->setSpacing(5);
	box0->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));
	box0->setFrameStyle(QFrame::Panel|QFrame::Sunken);
	lay0->addWidget(box0);

	QHBox *box1 = new QHBox(box0);
	box1->setSpacing(5);
	new QLabel(i18n("Destination file name"), box1);
	le_destname = new QLineEdit(box1);
	QPushButton *b_browse = new QPushButton(i18n("Browse"), box1);
	connect(b_browse, SIGNAL(clicked()), this, SLOT(browse()));

	QHBox *box2 = new QHBox(box0);
	box2->setSpacing(5);
	new QLabel(i18n("Archive type"), box2);
	cm_archtype = new QComboBox(box2);
	cm_archtype->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));

	struct { ImgArchiveSink::ArchiveType t; char *name; } names[] = {
		{ImgArchiveSink::RAR_ARCHIVE, "RAR"},
		{ImgArchiveSink::ZIP_ARCHIVE, "ZIP"},
		{ImgArchiveSink::ACE_ARCHIVE, "ACE"},
		{ImgArchiveSink::TARBZ2_ARCHIVE, "Bzipped tar"},
		{ImgArchiveSink::TARGZ_ARCHIVE, "Gzipped tar"},
		{ImgArchiveSink::UNKNOWN_ARCHIVE, NULL}};
	int supp = ImgArchiveSink::supportedCreateArchives();

	int n = 0;
	for (int i=0; names[i].name; i++)
		if (supp & names[i].t)
		{
			cm_archtype->insertItem(names[i].name);
			map.insert(++n, names[i].t); 
		}
	connect(cm_archtype, SIGNAL(activated(int)), this, SLOT(archiveTypeChanged(int)));

	lay0->addStretch();
	QHBoxLayout *lay3 = new QHBoxLayout(NULL, 5, 5);
	QPushButton *b_cancel = new QPushButton(i18n("Cancel"), this);
	QPushButton *b_create = new QPushButton(i18n("Create"), this);
	lay3->addStretch();
	lay3->addWidget(b_create);
	lay3->addWidget(b_cancel);
	lay0->addLayout(lay3);
}

ArchiverDialog::~ArchiverDialog()
{
}

void ArchiverDialog::browse()
{        
	const QString file = QFileDialog::getSaveFileName(imgsink->getFullName(),
                        "Archives (" + ImgArchiveSink::supportedSaveExtensions() + ");;All files (*)",
                        this, i18n("Choose a file") );
        if (!file.isEmpty())
		le_destname->setText(file);
}

void ArchiverDialog::archiveTypeChanged(int index)
{
	if (map.contains(index))
	{
		QString newext = ImgArchiveSink::getExtension(map[index]);
		QStringList exts = ImgArchiveSink::getAllExtenstionsList();
		for (QStringList::const_iterator it = exts.begin(); it != exts.end(); it++)
		{
			if (le_destname->text().endsWith(*it)) //todo: ignore case
			{
				//todo: replace extension
			}
		}
	}
}

#include "archiverdialog.moc"
