#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "Surface.h"

SurfaceTriangle::SurfaceTriangle() {
	rainfall = 0;
	temperature = 0;
	AllAround = new SurfaceTriangle *[3];

	AllAround[0] = NULL;
	AllAround[1] = NULL;
	AllAround[2] = NULL;
}

SurfaceTriangle *SurfaceTriangle::Around(int index) {
	if(AllAround == NULL)
		return NULL;

	if(index >= 3 || index < 0)
		return NULL;

	return AllAround[index];
}

void SurfaceTriangle::SetAround(int index, SurfaceTriangle * target) {
	if(AllAround == NULL)
		return;

	if(index >= 3 || index < 0)
		return;

	 AllAround[index] = target;
}

void SurfaceTriangle::Subdivide() {
	SurfaceTriangle *tmp;

	tmp = AllAround[2];

	AllAround[2] = new SurfaceTriangle();

	AllAround[2]->SetAround(0, this);
	AllAround[2]->SetAround(1, new SurfaceTriangle());
	AllAround[2]->SetAround(2, new SurfaceTriangle());

	AllAround[2]->Around(1)->SetAround(0, this->Around(2));
	AllAround[2]->Around(1)->SetAround(1, this->Around(1));
	AllAround[2]->Around(1)->SetAround(2, tmp);

	AllAround[2]->Around(2)->SetAround(0, this->Around(2));
	AllAround[2]->Around(2)->SetAround(1, tmp);
	AllAround[2]->Around(2)->SetAround(2, this->Around(0));

}
