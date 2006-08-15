/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <yogin@linux.bydg.org>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "imlibimage.h"
#include <qpaintdevice.h>
#include <qfile.h>
#include <X11/Xlib.h>
#include <Imlib2.h>

using namespace QComicBook;

QMutex ImlibImage::mutex;

ImlibImage::ImlibImage(): data(NULL)
{
	mutex.lock();
	context = imlib_context_new();
	mutex.unlock();
}

ImlibImage::~ImlibImage()
{
	mutex.lock();
	if (data)
	{
		imlib_context_push(context);
		imlib_context_set_image(data);
		imlib_free_image();
		imlib_context_pop(); //??
	}
	imlib_context_free(context);
	mutex.unlock();
}

int ImlibImage::load(const QString &path)
{
	ImlibLoadError error;

	mutex.lock();

	imlib_context_push(context);
	data = imlib_load_image_with_error_return(QFile::encodeName(path), &error);
	imlib_context_set_image(data);
	w = imlib_image_get_width();
	h = imlib_image_get_height();
	imlib_context_pop();

	mutex.unlock();
	return 1;
}

void ImlibImage::draw(QPaintDevice *p, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh)
{
	if (data)
	{
		mutex.lock();
		imlib_context_push(context);

		imlib_context_set_image(data);

		imlib_context_set_display(p->x11Display());
		imlib_context_set_visual((Visual *)p->x11Visual());
		imlib_context_set_colormap(p->x11Colormap());

		imlib_context_set_drawable(p->handle());
		imlib_render_image_part_on_drawable_at_size(sx, sy, sw, sh, dx, dy, dw, dh);
		
		imlib_context_pop();
		mutex.unlock();
	}
}

int ImlibImage::width() const
{
	return w;
}

int ImlibImage::height() const
{
	return h;
}

void ImlibImage::rotate(int orient)
{
	if (data)
	{
		mutex.lock();

		imlib_context_push(context);
		imlib_context_set_image(data);
		imlib_image_orientate(orient);
		w = imlib_image_get_width();
		h = imlib_image_get_height();
		imlib_context_pop();

		mutex.unlock();
	}
}

ImlibImage* ImlibImage::rotateClone(int orient)
{
	return NULL;
	/*
	   ImlibImage *img = new ImlibImage();
	   imlib_context_push(context);
	   imlib_context_set_image(data);
	   imlib_image_orientate(orient);
	   img->data = imlib_clone_image();

	imlib_context_set_image(img->data);
	imlib_image_orientate(0);
	img->w = h; //imlib_image_get_width();
	img->h = w; //imlib_image_get_height();
	imlib_context_pop();
	return img;
	*/
}

void ImlibImage::reset()
{
	if (data)
	{
		mutex.lock();

		imlib_context_push(context);
		imlib_context_set_image(data);
		imlib_free_image();
		data = NULL;
		w = h = 0;
		imlib_context_pop();

		mutex.unlock();
	}
}


void ImlibImage::setCacheSize(int bytes)
{
	mutex.lock();
	imlib_set_cache_size(bytes);
	mutex.unlock();
}

