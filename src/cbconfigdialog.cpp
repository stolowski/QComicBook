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

ComicBookCfgDialog::ComicBookCfgDialog(QWidget *parent, ComicBookSettings *cfg): QTabDialog(parent), cfg(cfg)/*{{{*/
{
	setCaption("QComicBook Settings");
	setModal(true);

	setupDisplayTab();
	setupMiscTab();

	setCancelButton();
	setOkButton();
	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(apply()));
	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(cancel()));
}/*}}}*/

ComicBookCfgDialog::~ComicBookCfgDialog()/*{{{*/
{
}/*}}}*/

void ComicBookCfgDialog::setupDisplayTab()/*{{{*/
{
	QWidget *w = new QWidget(this);
	QVBoxLayout *lay = new QVBoxLayout(w, 5, 5);

	QVBox *box0 = new QVBox(w);
	lay->addWidget(box0);

	//
	// background color
	QHBox *box1 = new QHBox(box0);
	new QLabel(tr("Background color"), box1);
	pb_color = new QPushButton(box1);
	pb_color->setFixedWidth(32);
	pb_color->setPaletteBackgroundColor(bgcolor = cfg->getBackground());
	connect(pb_color, SIGNAL(clicked()), this, SLOT(showBackgroundDialog()));

	//QVGroupBox *gr_fullscr = new QVGroupBox(tr("Fullscreen mode"), box0);
	cb_hidemenu = new QCheckBox(tr("Hide menubar in fullscreen mode"),box0);
	cb_hidemenu->setChecked(cfg->getFullScreenHideMenu());
	/*cb_hidetoolbar = new QCheckBox(tr("Hide toolbar"), gr_fullscr);
	cb_hidetoolbar->setChecked(cfg->getFullScreenHideToolbar());
	cb_hidescrollbar = new QCheckBox(tr("Hide scrollbars"), gr_fullscr);
	cb_hidescrollbar->setChecked(cfg->getFullScreenHideScrollbars());*/
	
	//
	// scaling method
	QButtonGroup *gr_scaling = new QButtonGroup(2, Qt::Horizontal, tr("Scaling method"), box0);
	rb_smooth = new QRadioButton(tr("Smooth"), gr_scaling);
	rb_fast = new QRadioButton(tr("Fast"), gr_scaling);
	if (cfg->getScaling() == ComicImageView::Fast)
		rb_fast->setChecked(true);
	else
		rb_smooth->setChecked(true);
	
	lay->addStretch();
	
	addTab(w, tr("Display"));
}/*}}}*/

void ComicBookCfgDialog::setupMiscTab()/*{{{*/
{
	QWidget *w = new QWidget(this);
	QVBoxLayout *lay = new QVBoxLayout(w, 5, 5);

	QVBox *box0 = new QVBox(w);
	lay->addWidget(box0);

	QVGroupBox *grp0 = new QVGroupBox(tr("Cache"), box0);
	QHBox *box1 = new QHBox(grp0);
	new QLabel(tr("Cache size"), box1);
	sb_cachesize = new QSpinBox(0, 99, 1, box1);
	sb_cachesize->setValue(cfg->getCacheSize());
	cb_preload = new QCheckBox(tr("Preload next page"), grp0);
	cb_preload->setChecked(cfg->getPreload());

	QVGroupBox *grp1 = new QVGroupBox(tr("Thumbnails"), box0);
	cb_thumbs = new QCheckBox(tr("Use disk cache for thumbnails"), grp1);
	cb_thumbs->setChecked(cfg->getCacheThumbnails());
	QHBox *box2 = new QHBox(grp1);
	new QLabel(tr("Number of days to keep thumbnails"), box2);
	sb_thumbsage = new QSpinBox(0, 365, 1, box2);
	sb_thumbsage->setValue(cfg->getThumbnailsAge());
	
	cb_autoinfo = new QCheckBox(tr("Open info dialog after opening comic book"), box0);
	cb_autoinfo->setChecked(cfg->getAutoInfo());
	cb_confirmexit = new QCheckBox(tr("Confirm exit"), box0);
	cb_confirmexit->setChecked(cfg->getConfirmExit());

	lay->addStretch();
	addTab(w, tr("Misc"));
}/*}}}*/

void ComicBookCfgDialog::apply()/*{{{*/
{
	// display
	cfg->setBackground(bgcolor);
	cfg->setFullScreenHideMenu(cb_hidemenu->isChecked());
	if (rb_smooth->isChecked())
		cfg->setScaling(ComicImageView::Smooth);
	else
		if (rb_fast->isChecked())
			cfg->setScaling(ComicImageView::Fast);

	//
	// misc
	cfg->setCacheSize(sb_cachesize->value());
	cfg->setPreload(cb_preload->isChecked());
	cfg->setCacheThumbnails(cb_thumbs->isChecked());
	cfg->setThumbnailsAge(sb_thumbsage->value());
	cfg->setConfirmExit(cb_confirmexit->isChecked());
	cfg->setAutoInfo(cb_autoinfo->isChecked());
}/*}}}*/

void ComicBookCfgDialog::cancel()/*{{{*/
{
}/*}}}*/

void ComicBookCfgDialog::showBackgroundDialog()/*{{{*/
{
	QColor c = QColorDialog::getColor(cfg->getBackground(), this);
	if (c.isValid())
		pb_color->setPaletteBackgroundColor(bgcolor = c);
}/*}}}*/

