// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include <stdio.h>
#include <math.h>
#include "Surface.h"

int main(int argc, char* argv[]) {
//	// 1d4 for testing
//	SurfaceTriangle *triangle1 = new SurfaceTriangle();
//
//	triangle1->SetAround(0, new SurfaceTriangle());
//	triangle1->SetAround(1, new SurfaceTriangle());
//	triangle1->SetAround(2, new SurfaceTriangle());
//
//	SurfaceTriangle *triangle2 = triangle1->Around(0);
//	SurfaceTriangle *triangle3 = triangle1->Around(1);
//	SurfaceTriangle *triangle4 = triangle1->Around(2);
//
//	triangle2->SetAround(0, triangle1);
//	triangle2->SetAround(1, triangle3);
//	triangle2->SetAround(2, triangle4);
//
//	triangle3->SetAround(0, triangle1);
//	triangle3->SetAround(1, triangle4);
//	triangle3->SetAround(2, triangle2);
//
//	triangle4->SetAround(0, triangle1);
//	triangle4->SetAround(1, triangle2);
//	triangle4->SetAround(2, triangle3);
//

	SurfaceTriangle *level1 = new SurfaceTriangle();
	level1->CreateD20();
	SurfaceTriangle *level2, *level3, *level4, *level5, *dite;
	level2 = level1->SubdivideSurface();
	level3 = level2->SubdivideSurface();
	level4 = level3->SubdivideSurface();
	level5 = level4->SubdivideSurface();

	dite = level5;


	int out1 = dite->WaveTagCtrl234(2, 0);

	SurfaceTriangle * bodRovniku = dite->FindFirstTagValue(2, out1/2);
	int out2 = bodRovniku->WaveTagCtrl234(3, 0);

	dite->PrintSurface();

	printf("Under von Construction, %d %d\n", out1, out2);
	//getchar();
	//getchar();
}
