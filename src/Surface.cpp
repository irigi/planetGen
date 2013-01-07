#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "Surface.h"

int SurfaceTriangle::_id_counter = 0;

PhysicalData::PhysicalData() {
	temperature = 0;
	rainfall = 0;
	solarConstant = 0;
}

SurfaceTriangle::SurfaceTriangle() {
	AllAround = new SurfaceTriangle *[3];

	// is never NULL, there is always neighbor, at first, we connect it with itself
	AllAround[0] = this;
	AllAround[1] = this;
	AllAround[2] = this;

	u_coordinate = 0;
	v_coordinate = 0;

	_intControl1 = SURFACE_CONTROL_CLEAN;
	_intControl2 = 0;
	_intControl3 = 0;
	_divideImunity = false;

	data = new PhysicalData();
	_id = _id_counter; _id_counter++;
}

SurfaceTriangle::~SurfaceTriangle() {
	delete data;
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

	// we do not want to divide some cells
	if(_divideImunity)
		return;

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
	this->NullControlsInternal(SURFACE_CONTROL_INTERNAL, true);
	this->NullControlsInternal(SURFACE_CONTROL_CLEAN, true);
}

void SurfaceTriangle::FloodTagCtrl1(int tag) {
	if(SURFACE_CONTROL_INTERNAL == tag) {
		throw "Internal tag musn't be used in SurfaceTriangle::FloodTag!";
		return;
	}

	this->NullControlsInternal(tag);
}

void SurfaceTriangle::NullControlsInternal(const int stage, bool delete_234controls) {
	_intControl1 = stage;

	if(delete_234controls) {
		// these do not control null algorithm
		_intControl2 = 0;
		_intControl3 = 0;
	}

	// flood by stage
	if(AllAround[0]->IntControl1() != stage)
		AllAround[0]->NullControlsInternal(stage);

	if(AllAround[1]->IntControl1() != stage)
			AllAround[1]->NullControlsInternal(stage);

	if(AllAround[2]->IntControl1() != stage)
			AllAround[2]->NullControlsInternal(stage);
}

void SurfaceTriangle::SubdivideSurface() {
	this->FloodTagCtrl1(SURFACE_CONTROL_TAG1);
	this->SubdivideSurfaceInternal();
}

void SurfaceTriangle::SubdivideSurfaceInternal() {
	if(_intControl1 == SURFACE_CONTROL_TAG1)
			_intControl1 = SURFACE_CONTROL_CLEAN;
		else
			return;

	// flood subdivide
	if(AllAround[0]->IntControl1() == SURFACE_CONTROL_TAG1) {
		AllAround[0]->SubdivideSurfaceInternal();
	}

	if(AllAround[1]->IntControl1() == SURFACE_CONTROL_TAG1) {
			AllAround[1]->SubdivideSurfaceInternal();
	}

	if(AllAround[2]->IntControl1() == SURFACE_CONTROL_TAG1) {
			AllAround[2]->SubdivideSurfaceInternal();
	}

	this->SubdivideThis();
}

int SurfaceTriangle::GetTagCtrl234(int ctrl_index) {
	// illegal index variable
	if(ctrl_index <= 1 || ctrl_index > MaxIntCtrl)
		return -1;

	switch(ctrl_index) {
	case 2:
		return _intControl2;
		break;
	case 3:
		return _intControl3;
		break;
	default:
		throw "shouldn't happen in FloodTagCtrl234";
		return -2;
	}

}


void SurfaceTriangle::FloodTagCtrl234(int ctrl_index, int tag) {
	// illegal index variable
	if(ctrl_index <= 1 || ctrl_index > MaxIntCtrl)
		return;

	int *control = NULL;

	switch(ctrl_index) {
	case 2:
		control = &_intControl2;
		break;
	case 3:
		control = &_intControl3;
		break;
	default:
		throw "shouldn't happen in FloodTagCtrl234";
		return;
	}

	if(*control == tag)
		return;
	else
		*control = tag;

	// flood subdivide
	AllAround[0]->FloodTagCtrl234(ctrl_index, tag);
	AllAround[1]->FloodTagCtrl234(ctrl_index, tag);
	AllAround[2]->FloodTagCtrl234(ctrl_index, tag);
}

int SurfaceTriangle::WaveTagCtrl234(int ctrl_index, int tag) {
	return this->WaveTagCtrl234Internal(ctrl_index, tag);
}

int SurfaceTriangle::WaveTagCtrl234Internal(int ctrl_index, int tag, bool prvni) {
	// return maximum tag on the surface

	// illegal index variable
	if(ctrl_index <= 1 || ctrl_index > MaxIntCtrl)
		return tag-2;

	int *control = NULL;
	const long int almost_infinity = ALMOST_INFINITY;

	switch(ctrl_index) {
	case 2:
		control = &_intControl2;
		break;
	case 3:
		control = &_intControl3;
		break;
	default:
		throw "shouldn't happen in WaveTagCtrl234";
		return tag-2;
	}

	if(prvni) { // first call fills surface by almost infinity
		this->FloodTagCtrl234(ctrl_index, almost_infinity);
		*control = tag;
	} else {
		if(*control <= tag)
			return tag-2;
		else
			*control = tag;
	}

	// flood subdivide
	int a1 = AllAround[0]->WaveTagCtrl234Internal(ctrl_index, tag+1, false);
	int a2 = AllAround[1]->WaveTagCtrl234Internal(ctrl_index, tag+1, false);
	int a3 = AllAround[2]->WaveTagCtrl234Internal(ctrl_index, tag+1, false);

	return a1 > a2 ? (a1 > a3 ? a1 : a3) : (a2 > a3 ? a2 : a3);
}

int SurfaceTriangle::FollowLineTagCtrl234(int ctrl_index, int lead_index, int tag) {
	// return maximum tag on the line on the surface
	SurfaceTriangle * ret = FollowLineTagCtrl234Internal(ctrl_index, lead_index, tag, NULL, 0, 0);

	if(ret == NULL) {
		// this happens when error occurs
		printf("got null, vsechno zpet\n");
		this->NullControls();
		return 0;
	} else
		return ret->GetTagCtrl234(ctrl_index);
}

SurfaceTriangle * SurfaceTriangle::FollowLineTagCtrl234Internal(int ctrl_index, int lead_index, int tag, SurfaceTriangle * cameFrom, int minVal, int maxVal) {
	// Assumes there is an array in ctrl_leadindex that emerged from some wave algorithm.
	// It must well define "lines" on the surface, i.e. each triangle with ctrl_leadindex value  minVal must
	// have 2 neighboring triangles with value maxVal and vice versa. If this doesn't hold, the algorithm returns NULL.

	// illegal index variable
	if(ctrl_index <= 1 || ctrl_index > MaxIntCtrl)
		return NULL;

	if(lead_index <= 1 || lead_index > MaxIntCtrl || lead_index == ctrl_index)
		return NULL;

	int *control = NULL, *lead = NULL;
	const long int almost_infinity = ALMOST_INFINITY;

	switch(ctrl_index) {
	case 2:
		control = &_intControl2;
		break;
	case 3:
		control = &_intControl3;
		break;
	default:
		throw "shouldn't happen in FollowLineTagCtrl234";
		return NULL;
	}

	switch(lead_index) {
	case 2:
		lead = &_intControl2;
		break;
	case 3:
		lead = &_intControl3;
		break;
	default:
		throw "shouldn't happen in FollowLineTagCtrl234";
		return NULL;
	}

	if(cameFrom == NULL) { // first call fills surface by almost infinity
		this->FloodTagCtrl234(ctrl_index, almost_infinity);
		*control = tag;
		minVal = *lead - 1;
		maxVal = *lead + 1;
		printf("   zacatek %d %d, %d %d, %d -- %d %d\n", minVal, maxVal, _intControl2, _intControl3, _id, ctrl_index, lead_index);
	} else {
		if(*control <= tag || *lead < minVal || *lead > maxVal) {
			return this;
		} else {
			*control = tag;
			printf("   %d %d, %d %d, %d -- %d %d\n", minVal, maxVal, _intControl2, _intControl3, _id, ctrl_index, lead_index);
		}
	}

	SurfaceTriangle ** point = new SurfaceTriangle *[3];
	SurfaceTriangle * ret = NULL;

	point[0] = this; point[1] = this; point[2] =this;

	// flood subdivide
	for(int i = 0; i <3; i++) {
		printf("        soused %d, pan %d: %d %d\n", _id, AllAround[i]->GetID(), AllAround[i]->GetTagCtrl234(lead_index), AllAround[i]->GetTagCtrl234(ctrl_index));
		if(AllAround[i]->GetTagCtrl234(lead_index) >= minVal && AllAround[i]->GetTagCtrl234(lead_index) <= maxVal
				&& AllAround[i] != cameFrom && AllAround[i]->GetTagCtrl234(ctrl_index) > tag) {
			point[i] = AllAround[i]->FollowLineTagCtrl234Internal(ctrl_index, lead_index, tag+1, this, minVal, maxVal);
			break; // only one way around...
		}
	}

	if(point[0] == NULL || point[1] == NULL || point[2] == NULL)
		return NULL;

	ret = point[0];
	if(point[1]->GetTagCtrl234(ctrl_index) > ret->GetTagCtrl234(ctrl_index))
		ret = point[1];
	if(point[2]->GetTagCtrl234(ctrl_index) > ret->GetTagCtrl234(ctrl_index))
		ret = point[2];

	delete [] point;

	return ret;

}

SurfaceTriangle * SurfaceTriangle::FindFirstTagValue(int ctrl_index, int tag) {
	// illegal index variable
	if(ctrl_index <= 1 || ctrl_index > MaxIntCtrl)
		return NULL;

	int *control = NULL;
	const long int almost_infinity = ALMOST_INFINITY;

	switch(ctrl_index) {
	case 2:
		control = &_intControl2;
		break;
	case 3:
		control = &_intControl3;
		break;
	default:
		throw "shouldn't happen in WaveTagCtrl234";
		return NULL;
	}

	if(*control == tag)
		return this;

	// approach
	if(*control > tag) {
		if(AllAround[0]->GetTagCtrl234(ctrl_index) < *control)
			return AllAround[0]->FindFirstTagValue(ctrl_index, tag);
		if(AllAround[1]->GetTagCtrl234(ctrl_index) < *control)
					return AllAround[1]->FindFirstTagValue(ctrl_index, tag);
		if(AllAround[2]->GetTagCtrl234(ctrl_index) < *control)
					return AllAround[2]->FindFirstTagValue(ctrl_index, tag);
	} else {
		if(AllAround[0]->GetTagCtrl234(ctrl_index) > *control)
			return AllAround[0]->FindFirstTagValue(ctrl_index, tag);
		if(AllAround[1]->GetTagCtrl234(ctrl_index) > *control)
					return AllAround[1]->FindFirstTagValue(ctrl_index, tag);
		if(AllAround[2]->GetTagCtrl234(ctrl_index) > *control)
					return AllAround[2]->FindFirstTagValue(ctrl_index, tag);
	}

	return NULL;

}

void SurfaceTriangle::PrintSurface() {
	this->FloodTagCtrl1(SURFACE_CONTROL_TAG1);
	this->PrintSurfaceInternal();
}

void SurfaceTriangle::PrintSurfaceInternal() {
	if(_intControl1 == SURFACE_CONTROL_TAG1) {
		printf("%d (%d %d), %d\n",_intControl1, _intControl2, _intControl3, _id);
		printf("  soused - (%d %d), %d\n",AllAround[0]->GetTagCtrl234(2),AllAround[0]->GetTagCtrl234(3), AllAround[0]->GetID());
		printf("  soused - (%d %d), %d\n",AllAround[1]->GetTagCtrl234(2),AllAround[1]->GetTagCtrl234(3), AllAround[1]->GetID());
		printf("  soused - (%d %d), %d\n",AllAround[2]->GetTagCtrl234(2),AllAround[2]->GetTagCtrl234(3), AllAround[2]->GetID());
	} else return;

	_intControl1 = SURFACE_CONTROL_CLEAN;

	AllAround[0]->PrintSurfaceInternal();
	AllAround[1]->PrintSurfaceInternal();
	AllAround[2]->PrintSurfaceInternal();
}