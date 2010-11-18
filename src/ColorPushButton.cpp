/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "ColorPushButton.h"
#include <QPainter>
#include <QPaintEvent>

ColorPushButton::ColorPushButton(QWidget *parent): QPushButton(parent)
{
}

ColorPushButton::ColorPushButton(const QString &text, QWidget *parent): QPushButton(parent)
{
}

QColor ColorPushButton::getColor() const
{
	return color;
}

void ColorPushButton::setColor(const QColor &c)
{
	color = c;
	update();
}
			
void ColorPushButton::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	p.fillRect(0, 0, width(), height(), color);
	p.end();
	e->accept();
}
