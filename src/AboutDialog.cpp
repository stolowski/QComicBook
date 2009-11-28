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

#include "AboutDialog.h"
#include <QLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QSizePolicy>
#include <QPixmap>

using namespace QComicBook;

AboutDialog::AboutDialog(QWidget *parent, const QString &caption, const QString &text, const QPixmap &logo): QDialog(parent)
{
    setupUi(this);
    setWindowTitle(caption);
    setText(text);
    if (!logo.isNull())
        about_logo->setPixmap(logo);
    
    connect(bclose, SIGNAL(clicked()), this, SLOT(accept()));
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::setText(const QString &text)
{
        about_text->setText(text);
}

