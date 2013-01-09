// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include <stdio.h>
#include <math.h>
#include "Bitmap.h"
#include "PhysicalData.h"

BitmapPixel::BitmapPixel() {
	this->data = new PhysicalData();
	control1 = 0;
	controlDouble = 0;

	u_coordinate = 0;
	v_coordinate = 0;
}

Bitmap::Bitmap() {
	x_size = 0;
	y_size = 0;
	bitmap = NULL;
}

int Bitmap::FromCoordsI(double u, double v, int alternative) {
	int i;

	switch(alternative) {
	default:
		i = int(round(u/2.0/M_PI*x_size));
		break;
	}

	return i;
}

int Bitmap::FromCoordsJ(double u, double v, int alternative) {
	int j;

	switch(alternative) {
	default:
		j = int(round((v/M_PI + 0.5)*y_size));
		break;
	}

	return j;
}

double Bitmap::ToCoordsU(int i, int j, int alternative) {
	double u;

	switch(alternative) {
	default:
		u = i/double(x_size)*2*M_PI;
		break;
	}

	return u;
}

double Bitmap::ToCoordsV(int i, int j, int alternative) {
	double v;

	switch(alternative) {
	default:
		v = (j/double(y_size)-0.5)*M_PI;
		break;
	}

	return v;
}
