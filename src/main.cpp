// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include <stdio.h>
#include <math.h>
#include "Surface.h"

int main(int argc, char* argv[]) {

	SurfaceTriangle *level1 = new SurfaceTriangle();
	level1->CreateD20();
	SurfaceTriangle *level2, *level3, *level4, *level5, *dite;
	level2 = level1->SubdivideSurface();
	level3 = level2->SubdivideSurface();
	level4 = level3->SubdivideSurface();
	level5 = level4->SubdivideSurface();

	dite = level5;

	dite->CreateSphericalCoordinates();

	dite->PrintSurface();

	printf("Under von Construction %lu %lu\n", sizeof(long double), sizeof(double));
}
