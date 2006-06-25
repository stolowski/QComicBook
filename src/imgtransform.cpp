#include "imgtransform.h"
#include <qimage.h>
#include <iostream>
#include <math.h>

using namespace QComicBook;

float ImgTransform::sin_lut[360];
float ImgTransform::cos_lut[360];
bool ImgTransform::lut_init = true;

ImgTransform::ImgTransform(QImage *image)
{
	this->img = image;
	w = img->width();
	h = img->height();

	const float pi = 3.14159f;
		
	//
	// initialize Hough transformation accumulator
	acu = new int* [w+h];
	for (int r=0; r<w+h; r++)
	{
		acu[r] = new int [360];
		for (int a=0; a<360; a++)
			acu[r][a] = 0;
	}

	//
	// initialize sin/cos lookup tables
	if (lut_init)
	{
		lut_init = false;
		for (int a=0; a<360; a++)
		{
			const float ang = (a*pi)/180.0f;
			sin_lut[a] = sin(ang);
			cos_lut[a] = cos(ang);
		}
	}
}

ImgTransform::~ImgTransform()
{
	for (int r=0; r<w+h; r++)
		delete [] acu[r];
	delete [] acu;
}

void ImgTransform::gaussianBlur()
{
}

void ImgTransform::enhanceEdges()
{
}

void ImgTransform::houghTransform()
{
}

void ImgTransform::doTransform()
{
	TransformBuffer<float> G(w, h);
	TransformBuffer<int> I(w, h);

	for (int y=0; y<h; y++)
		for (int x=0; x<w; x++)
			I(x, y) = qGray(img->pixel(x, y));
	// GAUSS
	for (int y=0; y<h-1; y++)
		for (int x=4; x<w-4; x++)
		{
			G(x, y) =    0.006f * I(x-3, y) +
				     0.061f * I(x-2, y) +
				     0.242f * I(x-1, y) + 
				     0.383f * I(x, y) +
				     0.242f * I(x+1, y) + 
				     0.061f * I(x+2, y) +
				     0.006f * I(x+3, y);
		}
	for (int x=0; x<w; x++)
		for (int y=4; y<h-4; y++)
		{
			G(x, y) =    0.006f * G(x, y-3) +
				     0.061f * G(x, y-2) +
				     0.242f * G(x, y-1) + 
				     0.383f * G(x, y) +
				     0.242f * G(x, y+1) + 
				     0.061f * G(x, y+2) +
				     0.006f * G(x, y+3);
			int c = (int)G(x, y);
			img->setPixel(x, y, qRgb(c, c, c));
		}

	for (int y=1; y<h-1; y++)
		for (int x=1; x<w-1; x++)
		{
			int h, v;
			// horiz.
			h = -G(x-1, y-1);
			h += G(x+1, y-1);
			h -= 2*G(x-1, y);
			h += 2*G(x+1, y);
			h -= G(x-1, y+1);
			h += G(x+1, y+1);
			h /= 9;

			if (h<0)
				h = -h;
			
			v = G(x-1, y-1);
			v += 2*G(x, y-1);
			v += G(x+1, y-1);
			v -= G(x-1, y+1);
			v -= 2*G(x, y+1);
			v -= G(x+1, y+1);
			v /= 9;

			if (v<0)
				v = -v;
			h += v;
			if (h>255)
				h=255;
			if (h<20)
				h=0;
			I(x, y) = h;
		}


	for (int y=1; y<h-1; y++)
		for (int x=1; x<w-1; x++)
		{
			int c = I(x, y);
			img->setPixel(x, y, qRgb(c, c, c));
		}

	// hough
	for (int y=4; y<h-1; y++)
	{
		for (int x=8; x<w-1; x++)
		{
			for (int a=0; a<360; a++)
				if ((a<5) || (a>88 && a<92) || (a>178 && a<182) || (a>268 && a<272))
				{
					float r = (float)x * cos_lut[a] + (float)y * sin_lut[a];
					int d = (int)r + (w+h)/2;
					if (d>0 && d<w+h)
						acu[d][(int)a]++;
				}
		}
	}
	for (int r=0; r<w+h; r++)
	{
		for (int a=0; a<360; a++)
		{
			std::cout << acu[r][a] << " ";
		}
		std::cout << "\n";
	}
}

