/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include <QIntValidator>
#include "GoToPageWidget.h"

using namespace QComicBook;

GoToPageWidget::GoToPageWidget(QWidget *parent, int value, int max): QDialog(parent)
{
    setupUi(this);

    QIntValidator *validator = new QIntValidator(1, max, this);
    le_pagenumber->setValidator(validator);
    if (value < 1)
    {
        value = 1;
    }
    else if (value > max)
    {
        value = max;
    }
    le_pagenumber->setText(QString::number(value));

    connect(le_pagenumber, SIGNAL(returnPressed()), this, SLOT(goClicked()));
    lb_total->setText(QString("/") + QString::number(max));
    connect(pb_go, SIGNAL(clicked()), this, SLOT(goClicked()));
    pb_go->setDefault(true);
}

GoToPageWidget::~GoToPageWidget()
{
}

void GoToPageWidget::goClicked()
{
	int n = le_pagenumber->text().toInt() - 1;
        accept();
	emit pageSelected(n);
}

