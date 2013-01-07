// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include <stdio.h>
#include <math.h>
#include "Surface.h"

int main(int argc, char* argv[]) {
	// 1d4 for testing
	SurfaceTriangle *triangle1 = new SurfaceTriangle();

	triangle1->SetAround(0, new SurfaceTriangle());
	triangle1->SetAround(1, new SurfaceTriangle());
	triangle1->SetAround(2, new SurfaceTriangle());

	SurfaceTriangle *triangle2 = triangle1->Around(0);
	SurfaceTriangle *triangle3 = triangle1->Around(1);
	SurfaceTriangle *triangle4 = triangle1->Around(2);

	triangle2->SetAround(0, triangle1);
	triangle2->SetAround(1, triangle3);
	triangle2->SetAround(2, triangle4);

	triangle3->SetAround(0, triangle1);
	triangle3->SetAround(1, triangle4);
	triangle3->SetAround(2, triangle2);

	triangle4->SetAround(0, triangle1);
	triangle4->SetAround(1, triangle2);
	triangle4->SetAround(2, triangle3);


	triangle1->SubdivideSurface();
	triangle1->SubdivideSurface();

	triangle1->NullControls();

	printf("Under von Construction, %lu\n", sizeof(SurfaceTriangle));

	getchar();
	getchar();
}
