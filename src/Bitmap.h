// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

class PhysicalData;

class BitmapPixel {
public:
	BitmapPixel();
	PhysicalData * data;

	int control1;
	double controlDouble;
	double u_coordinate, v_coordinate;
};

class Bitmap {
public:
	Bitmap(int x_size, int y_size);
	BitmapPixel *** bitmap;
	int x_size, y_size;

	int FromCoordsI(double u, double v, int alternative = -1);
	int FromCoordsJ(double u, double v, int alternative = -1);
	double ToCoordsU(int i, int j, int alternative = -1);
	double ToCoordsV(int i, int j, int alternative = -1);

	void WaveDefinedPixelsSpherical(); // floods pixels from projected surface into empty ones using curved metric of the coordinate grid

private:
	void WaveDefinedPixelsSphericalInternal();
};
