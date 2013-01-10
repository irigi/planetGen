// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include <stdio.h>
#include <math.h>
#include "Bitmap.h"
#include "PhysicalData.h"
#include "UserSettings.h"

BitmapPixel::BitmapPixel() {
	control1 = 0;
	controlFloat = 0;
	value = 0;
}

Bitmap::Bitmap(int x, int y, int projection) {
	x_size = x;
	y_size = y;
	coordinate_projection = projection;

	if(x_size <= 0 || y_size <= 0) {
		bitmap = NULL;
		x_size = 0;
		y_size = 0;
	} else {
		bitmap = new BitmapPixel **[x_size];

		for(int i = 0; i < x_size; i++) {
			bitmap[i] = new BitmapPixel *[y_size];

			for(int j = 0; j < y_size; j++) {
				bitmap[i][j] = new BitmapPixel();
			}
		}
	}
}

int BitmapPixel::FromCoordsI(double u, double v, int alternative, int size_x, int size_y) {
	int i;

	switch(alternative) {
	default:
		i = int(round(u/2.0/M_PI*size_x));
		break;
	}

	return i;
}

int BitmapPixel::FromCoordsJ(double u, double v, int alternative, int size_x, int size_y) {
	int j;

	switch(alternative) {
	default:
		j = int(round((v/M_PI + 0.5)*size_y));
		break;
	}

	return j;
}

double BitmapPixel::ToCoordsU(int i, int j, int alternative, int size_x, int size_y) {
	double u;

	switch(alternative) {
	default:
		u = i/double(size_x)*2*M_PI;
		break;
	}

	return u;
}

double BitmapPixel::ToCoordsV(int i, int j, int alternative, int size_x, int size_y) {
	double v;

	switch(alternative) {
	default:
		v = (j/double(size_y)-0.5)*M_PI;
		break;
	}

	return v;
}

void Bitmap::WaveDefinedPixelsSpherical() {
	if(bitmap == NULL)
		return;

	const int max_steps = 100;

	for(int i = 0; i < x_size; i++) {
		for(int j = 0; j < y_size;j++) {
			if(bitmap[i][j]->control1 == 1) {
				for(int k = 0; k < max_steps; k++) {
					float max = (k+1) * 3*M_PI/max_steps;
					this->WaveDefinedPixelsSphericalInternal(i, j, 0, max);
				}
			}
		}
	}

}

void Bitmap::WaveDefinedPixelsSphericalInternal(int i, int j, float tag, float max) {
	if(bitmap == NULL || i < 0 || j < 0 || i >= x_size || j  >= y_size)
		return;

	if(tag > max)
		return;

	if(bitmap[i][j]->controlFloat <= tag)
		return;
	else {
		bitmap[i][j]->controlFloat = tag;
	}

	// too slow?
	float
		xdiff = fabs(ToCoordsU(i,j) - ToCoordsU(i+1,j)),
		ydiff = fabs(ToCoordsV(i,j) - ToCoordsV(i,j+1));

	WaveDefinedPixelsSphericalInternal(i - 1, j, tag + xdiff, max);
	WaveDefinedPixelsSphericalInternal(i + 1, j, tag + xdiff, max);
	WaveDefinedPixelsSphericalInternal(i, j - 1, tag + ydiff, max);
	WaveDefinedPixelsSphericalInternal(i, j + 1, tag + ydiff, max);

}
