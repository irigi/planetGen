// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#define MAP_PROJECTION_DEFAULT -1

class PhysicalData;

class BitmapPixel {
public:
	BitmapPixel();

	// bitmaps are BIG, so remember to keep all this slim, 40 bytes x 4096 x 2048 = 320 MB of RAM already
	float value;
	short int control1;
	float controlFloat;

	static int FromCoordsI(double u, double v, int alternative, int size_x, int size_y);
	static int FromCoordsJ(double u, double v, int alternative, int size_x, int size_y);
	static double ToCoordsU(int i, int j, int alternative, int size_x, int size_y);
	static double ToCoordsV(int i, int j, int alternative, int size_x, int size_y);
};

class Bitmap {
public:
	Bitmap(int x_size, int y_size, int projection);
	BitmapPixel *** bitmap;
	int x_size, y_size;
	int coordinate_projection;

	int FromCoordsI(double u, double v) { return BitmapPixel::FromCoordsI(u, v, coordinate_projection, x_size, y_size); };
	int FromCoordsJ(double u, double v) { return BitmapPixel::FromCoordsJ(u, v, coordinate_projection, x_size, y_size); };
	double ToCoordsU(int i, int j) { return BitmapPixel::ToCoordsU(i, j, coordinate_projection, x_size, y_size); };
	double ToCoordsV(int i, int j) { return BitmapPixel::ToCoordsV(i, j, coordinate_projection, x_size, y_size); };

	void WaveDefinedPixelsSpherical(); // floods pixels from projected surface into empty ones using curved metric of the coordinate grid

private:
	void WaveDefinedPixelsSphericalInternal(int i, int j, float tag, float max);
};
