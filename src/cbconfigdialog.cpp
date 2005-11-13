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

#include "cbconfigdialog.h"
#include "cbsettings.h"
#include "imgview.h"
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qvgroupbox.h>
#include <qspinbox.h>
#include <qfontdialog.h>
#include <qfiledialog.h>
#include <qlineedit.h>

using namespace QComicBook;

ComicBookCfgDialog::ComicBookCfgDialog(QWidget *parent, ComicBookSettings *cfg): QTabDialog(parent), cfg(cfg)
{
	setCaption("QComicBook Settings");
	setModal(true);

	setupDisplayTab();
	setupMiscTab();
	//setupEditTab();

	setCancelButton();
	setOkButton();

	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(apply()));
	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(cancel()));
}

ComicBookCfgDialog::~ComicBookCfgDialog()
{
}

void ComicBookCfgDialog::setupDisplayTab()
{
	font = cfg->infoFont();

	QWidget *w = new QWidget(this);
	QVBoxLayout *lay = new QVBoxLayout(w, 5, 5);

	//
	// background color
	QHBoxLayout *lay1 = new QHBoxLayout;
	lay1->addWidget(new QLabel(tr("Background color"), w));
	lay1->addWidget(pb_color = new QPushButton(w));
	pb_color->setFixedWidth(32);
	pb_color->setPaletteBackgroundColor(bgcolor = cfg->background());
	connect(pb_color, SIGNAL(clicked()), this, SLOT(showBackgroundDialog()));
	lay->addLayout(lay1);

	cb_hidemenu = new QCheckBox(tr("Hide menubar in fullscreen mode"), w);
	cb_hidemenu->setChecked(cfg->fullScreenHideMenu());
	lay->addWidget(cb_hidemenu);

	cb_hidestatus = new QCheckBox(tr("Hide statusbar in fullscreen mode"), w);
	cb_hidestatus->setChecked(cfg->fullScreenHideStatusbar());
	lay->addWidget(cb_hidestatus);

	cb_smallcursor = new QCheckBox(tr("Small mouse cursor"), w);
	cb_smallcursor->setChecked(cfg->smallCursor());
	lay->addWidget(cb_smallcursor);
		
	//
	// scaling method
	QButtonGroup *gr_scaling = new QButtonGroup(2, Qt::Horizontal, tr("Scaling method"), w);
	rb_smooth = new QRadioButton(tr("Smooth"), gr_scaling);
	rb_fast = new QRadioButton(tr("Fast"), gr_scaling);
	if (cfg->pageScaling() == Fast)
		rb_fast->setChecked(true);
	else
		rb_smooth->setChecked(true);
	lay->addWidget(gr_scaling);

	//
	// font
	QHBoxLayout *lay3 = new QHBoxLayout;
	lay3->addWidget(new QLabel(tr("Info text font"), w));
	fontname = new QLabel(w);
	fontname->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	fontname->setLineWidth(1);
	lay3->addWidget(fontname, 1);
	QPushButton *pb_font = new QPushButton(tr("Choose"), w);
	connect(pb_font, SIGNAL(clicked()), this, SLOT(showFontDialog()));
	lay3->addWidget(pb_font);
	lay->addLayout(lay3);
	
	lay->addStretch();
	updateFontPreview();
	
	addTab(w, tr("Display"));
}

void ComicBookCfgDialog::setupMiscTab()
{
	bool f;

	QWidget *w = new QWidget(this);
	QVBoxLayout *lay = new QVBoxLayout(w, 5, 5);

	QVGroupBox *grp0 = new QVGroupBox(tr("Cache"), w);
	QHBox *box1 = new QHBox(grp0);
	new QLabel(tr("Cache size"), box1);
	sb_cachesize = new QSpinBox(0, 99, 1, box1);
	sb_cachesize->setValue(cfg->cacheSize());
	cb_preload = new QCheckBox(tr("Preload next page"), grp0);
	cb_preload->setChecked(cfg->preloadPages());
	lay->addWidget(grp0);

	QVGroupBox *grp1 = new QVGroupBox(tr("Thumbnails"), w);
	cb_thumbs = new QCheckBox(tr("Use disk cache for thumbnails"), grp1);
	cb_thumbs->setChecked(cfg->cacheThumbnails());
	QHBox *box2 = new QHBox(grp1);
	new QLabel(tr("Number of days to keep thumbnails"), box2);
	sb_thumbsage = new QSpinBox(0, 365, 1, box2);
	sb_thumbsage->setValue(cfg->thumbnailsAge());
	lay->addWidget(grp1);
	
	QVGroupBox *grp2 = new QVGroupBox(tr("Help browser"), w);
	cb_intbrowser = new QCheckBox(tr("Use Built-in browser"), grp2);
	QWidget *box3 = new QWidget(grp2);
	QHBoxLayout *lay3 = new QHBoxLayout(box3, 0, 5);
	lay3->addWidget(new QLabel(tr("External browser"), box3));
	lay3->addWidget(le_extbrowser = new QLineEdit(box3));
	lay3->addWidget(pb_brbrowse = new QPushButton(tr("Browse"), box3));
	connect(pb_brbrowse, SIGNAL(clicked()), this, SLOT(browseExternalBrowser()));
	lay->addWidget(grp2);
	connect(cb_intbrowser, SIGNAL(toggled(bool)), this, SLOT(browserCheckboxToggled(bool)));
	cb_intbrowser->setChecked(f = cfg->useInternalBrowser());
	browserCheckboxToggled(f);
	le_extbrowser->setText(cfg->externalBrowser());

	cb_twopagesstep = new QCheckBox(tr("Forward and backward two pages in two-pages mode"), w);
	cb_twopagesstep->setChecked(cfg->twoPagesStep());
	lay->addWidget(cb_twopagesstep);
	
	cb_autoinfo = new QCheckBox(tr("Open info dialog after opening comic book"), w);
	cb_autoinfo->setChecked(cfg->autoInfo());
	lay->addWidget(cb_autoinfo);

	cb_confirmexit = new QCheckBox(tr("Confirm exit"), w);
	cb_confirmexit->setChecked(cfg->confirmExit());
	lay->addWidget(cb_confirmexit);

	lay->addStretch();
	addTab(w, tr("Misc"));
}

void ComicBookCfgDialog::setupEditTab()
{
	QWidget *w = new QWidget(this);
	QVBoxLayout *lay = new QVBoxLayout(w, 5, 5);

	cb_editing = new QCheckBox(tr("Enable editing"), w);
	cb_editing->setChecked(cfg->editSupport());
	lay->addWidget(cb_editing);

	lay->addStretch();
	addTab(w, tr("Editing"));
}

void ComicBookCfgDialog::apply()
{
	//
	// display
	cfg->background(bgcolor);
	cfg->fullScreenHideMenu(cb_hidemenu->isChecked());
	cfg->fullScreenHideStatusbar(cb_hidestatus->isChecked());
	cfg->smallCursor(cb_smallcursor->isChecked());
	if (rb_smooth->isChecked())
		cfg->pageScaling(Smooth);
	else
		if (rb_fast->isChecked())
			cfg->pageScaling(Fast);
	cfg->infoFont(font);

	//
	// misc
	cfg->cacheSize(sb_cachesize->value());
	cfg->preloadPages(cb_preload->isChecked());
	cfg->cacheThumbnails(cb_thumbs->isChecked());
	cfg->thumbnailsAge(sb_thumbsage->value());
	cfg->twoPagesStep(cb_twopagesstep->isChecked());
	cfg->useInternalBrowser(cb_intbrowser->isChecked());
	cfg->externalBrowser(le_extbrowser->text());
	cfg->autoInfo(cb_autoinfo->isChecked());
	cfg->confirmExit(cb_confirmexit->isChecked());

	//
	// edit
	//cfg->editSupport(cb_editing->isChecked());
}

void ComicBookCfgDialog::cancel()
{
}

void ComicBookCfgDialog::updateFontPreview()
{
	fontname->setText(font.family() + ", " + QString::number(font.pointSize()));
	fontname->setFont(font);
}

void ComicBookCfgDialog::showBackgroundDialog()
{
	QColor c = QColorDialog::getColor(cfg->background(), this);
	if (c.isValid())
		pb_color->setPaletteBackgroundColor(bgcolor = c);
}

void ComicBookCfgDialog::showFontDialog()
{
	bool ok;
	font = QFontDialog::getFont(&ok, font, this);
	if (ok)
		updateFontPreview();
}

void ComicBookCfgDialog::browseExternalBrowser()
{
	const QString file = QFileDialog::getOpenFileName(QString::null,
			"All files (*)", this, NULL, tr("Choose a file") );
	if (file != QString::null)
		le_extbrowser->setText(file);
}

void ComicBookCfgDialog::browserCheckboxToggled(bool f)
{
	le_extbrowser->setDisabled(f);
	pb_brbrowse->setDisabled(f);
}

