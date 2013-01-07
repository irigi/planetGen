#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "Surface.h"

SurfaceTriangle::SurfaceTriangle() {
	rainfall = 0;
	temperature = 0;
	AllAround = new SurfaceTriangle *[3];

	// is never NULL, there is always neighbor, at first, we connect it with itself
	AllAround[0] = this;
	AllAround[1] = this;
	AllAround[2] = this;

	_intControl1 = SURFACE_CONTROL_CLEAN;
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

void SurfaceTriangle::SubdivideThis() {
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

void SurfaceTriangle::NullControls() {
	this->NullControlsInternal(SURFACE_CONTROL_INTERNAL);
	this->NullControlsInternal(SURFACE_CONTROL_CLEAN);
}

void SurfaceTriangle::NullControlsInternal(const int stage) {
	printf("%d, %d\n",_intControl1, stage);
	_intControl1 = stage;

	// flood by stage
	if(AllAround[0]->IntControl1() != stage)
		AllAround[0]->NullControlsInternal(stage);

	if(AllAround[1]->IntControl1() != stage)
			AllAround[1]->NullControlsInternal(stage);

	if(AllAround[2]->IntControl1() != stage)
			AllAround[2]->NullControlsInternal(stage);
}
