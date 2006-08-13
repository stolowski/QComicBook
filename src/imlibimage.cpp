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
#include <iostream>
#include <qpaintdevice.h>
#include <qfile.h>
#include <X11/Xlib.h>
#include <Imlib2.h>

using namespace QComicBook;

ImlibImage::ImlibImage(): data(NULL)
{
	context = imlib_context_new();
}

ImlibImage::~ImlibImage()
{
	if (data)
	{
		imlib_context_push(context);
		imlib_context_set_image(data);
		imlib_free_image();
		imlib_context_pop(); //??
	}
	imlib_context_free(context);
}

int ImlibImage::load(const QString &path)
{
	ImlibLoadError error;
	std::cout << "loading: " << path << std::endl;
	imlib_context_push(context);
	data = imlib_load_image_with_error_return(QFile::encodeName(path), &error);
	imlib_context_set_image(data);
	w = imlib_image_get_width();
	h = imlib_image_get_height();
	imlib_context_pop();
	return 1;
}

void ImlibImage::draw(QPaintDevice *p, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh)
{
	std::cout << "sx=" << sx << " "
		  << "sy=" << sy << " "
		  << "sw=" << sw << " "
		  << "sh=" << sh << " "
		  << "dx=" << dx << " "
		  << "dy=" << dy << " "
		  << "dw=" << dw << " "
		  << "dh=" << dh << " "
		  << std::endl;

	if (data)
	{
		imlib_context_push(context);

		imlib_context_set_image(data);
		Display *disp = QPaintDevice::x11AppDisplay();
		imlib_context_set_display(disp);
		imlib_context_set_visual(DefaultVisual(disp, DefaultScreen(disp)));
		imlib_context_set_colormap(DefaultColormap(disp, DefaultScreen(disp)));
		imlib_context_set_drawable(p->handle());
		imlib_render_image_part_on_drawable_at_size(sx, sy, sw, sh, dx, dy, dw, dh);
		
		imlib_context_pop();
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
	std::cout << "angle=" << orient << "\n";
	if (data)
	{
		imlib_context_push(context);
		imlib_context_set_image(data);
		imlib_image_orientate(orient);
		w = imlib_image_get_width();
		h = imlib_image_get_height();
		imlib_context_pop();
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
		imlib_context_push(context);
		imlib_context_set_image(data);
		imlib_free_image();
		data = NULL;
		w = h = 0;
		imlib_context_pop();
	}
}


void ImlibImage::setCacheSize(int bytes)
{
	imlib_set_cache_size(bytes);
}

