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
