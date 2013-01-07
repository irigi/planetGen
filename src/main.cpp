// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include <stdio.h>
#include <math.h>
#include "Surface.h"

int main(int argc, char* argv[]) {
	SurfaceTriangle *triangle = new SurfaceTriangle;

	triangle->SubdivideThis();
	triangle->SubdivideThis();
	triangle->SubdivideThis();
	triangle->SubdivideThis();

	triangle->NullControls();

	printf("Under von Construction, %d\n", sizeof(SurfaceTriangle));

	getchar();
	getchar();
}
