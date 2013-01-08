#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "Surface.h"
#include "PhysicalData.h"

int SurfaceTriangle::_id_counter = 0;

SurfaceTriangle::SurfaceTriangle() {
	AllAround = new SurfaceTriangle *[3];

	// is never NULL, there is always neighbor, at first, we connect it with itself
	AllAround[0] = this;
	AllAround[1] = this;
	AllAround[2] = this;

	DivisionField = new SurfaceTriangle *[4];

	DivisionField[0] = NULL;
	DivisionField[1] = NULL;
	DivisionField[2] = NULL;
	DivisionField[3] = NULL;

	_intControl1 = SURFACE_CONTROL_CLEAN;
	_intControl2 = 0;
	_intControl3 = 0;
	_intControl4 = 0;
	_divideImunity = false;

	data = new PhysicalData();
	_id = _id_counter; _id_counter++;
}

SurfaceTriangle::~SurfaceTriangle() {
	delete data;
	delete [] AllAround;
	delete [] DivisionField;
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
		_intControl4 = 0;
	}

	// flood by stage
	if(AllAround[0]->IntControl1() != stage)
		AllAround[0]->NullControlsInternal(stage);

	if(AllAround[1]->IntControl1() != stage)
			AllAround[1]->NullControlsInternal(stage);

	if(AllAround[2]->IntControl1() != stage)
			AllAround[2]->NullControlsInternal(stage);
}

SurfaceTriangle * SurfaceTriangle::SubdivideSurface() {
	SurfaceTriangle * ret;

	this->FloodTagCtrl234(2, ALMOST_INFINITY);
	ret = this->SubdivideSurfaceInternal(NULL, NULL, NULL, 0);
	this->NullControls();
	return ret;
}

SurfaceTriangle * SurfaceTriangle::SubdivideSurfaceInternal(SurfaceTriangle *caller, SurfaceTriangle ** neighbor1, SurfaceTriangle ** neighbor2, int tag) {
//	printf("zavolan na list %d z listu %d\n", _id, caller == NULL ? -1 : caller->GetID());

	if(tag <= _intControl2)
		_intControl2 = tag;

	// if he doesn't divide, HE is the neighbor of all caller children
	if(_divideImunity) {
		*neighbor1 = this;
		*neighbor2 = this;

		return this;
	}


	// if there are no children, create them
	if(DivisionField[0] == NULL) {
//		printf("tvorim deti listu %d\n", _id);

		DivisionField[0] = new SurfaceTriangle();
		DivisionField[1] = new SurfaceTriangle();
		DivisionField[2] = new SurfaceTriangle();
		DivisionField[3] = new SurfaceTriangle();

		// set up bonds between them
		DivisionField[0]->SetAround(0, DivisionField[1]);
		DivisionField[0]->SetAround(1, DivisionField[2]);
		DivisionField[0]->SetAround(2, DivisionField[3]);

		DivisionField[1]->SetAround(0, DivisionField[0]);
		DivisionField[2]->SetAround(0, DivisionField[0]);
		DivisionField[3]->SetAround(0, DivisionField[0]);

//		printf("     %d %d %d %d\n", DivisionField[0]->GetID(),DivisionField[1]->GetID(),DivisionField[2]->GetID(),DivisionField[3]->GetID());
	}

	SurfaceTriangle * nei1 = NULL, *nei2 = NULL;

	// if we lack information of neighbouring children of AllAround[0], we ask for it and fill the info
	if((DivisionField[1]->Around(2) == DivisionField[1] || DivisionField[2]->Around(1) == DivisionField[2]) && tag <= _intControl2) {
//			printf("volam 1 (%d)\n", AllAround[0]->GetID());
			AllAround[0]->SubdivideSurfaceInternal(this, &nei1, &nei2, tag+1);
			DivisionField[1]->SetAround(2, nei2); // every big triangle is clockwise oriented
			DivisionField[2]->SetAround(1, nei1);
//			printf("  dostavam 1 (%d %d)\n", nei1->GetID(), nei2->GetID());
	}

	// if we lack information of neighbouring children of AllAround[1], we ask for it and fill the info
	if((DivisionField[2]->Around(2) == DivisionField[2] || DivisionField[3]->Around(1) == DivisionField[3]) && tag <= _intControl2) {
//			printf("volam 2 (%d)\n", AllAround[1]->GetID());
			AllAround[1]->SubdivideSurfaceInternal(this, &nei1, &nei2, tag+1);
			DivisionField[2]->SetAround(2, nei2); // every big triangle is clockwise oriented
			DivisionField[3]->SetAround(1, nei1);
//			printf("  dostavam 2 (%d %d)\n", nei1->GetID(), nei2->GetID());
	}

	// if we lack information of neighbouring children of AllAround[2], we ask for it and fill the info
	if((DivisionField[3]->Around(2) == DivisionField[3] || DivisionField[1]->Around(1) == DivisionField[1]) && tag <= _intControl2) {
//		printf("volam 3 (%d)\n", AllAround[2]->GetID());
			AllAround[2]->SubdivideSurfaceInternal(this, &nei1, &nei2, tag+1);
			DivisionField[3]->SetAround(2, nei2); // every big triangle is clockwise oriented
			DivisionField[1]->SetAround(1, nei1);
//			printf("  dostavam 3 (%d %d)\n", nei1->GetID(), nei2->GetID());
	}

	if(caller == NULL || neighbor1 == NULL || neighbor2 == NULL)
		return DivisionField[0];

	// in all cases inform the caller of its neighbors
	if(caller == AllAround[0]) {
//		printf("      vracim 1 (%d %d)\n", caller->GetID(), AllAround[0]->GetID());
		*neighbor1 = DivisionField[1];
		*neighbor2 = DivisionField[2];
	}
	if(caller == AllAround[1]) {
//		printf("      vracim 2 (%d %d)\n", caller->GetID(), AllAround[0]->GetID());
		*neighbor1 = DivisionField[2];
		*neighbor2 = DivisionField[3];
	}
	if(caller == AllAround[2]) {
//		printf("      vracim 3 (%d %d)\n", caller->GetID(), AllAround[0]->GetID());
		*neighbor1 = DivisionField[3];
		*neighbor2 = DivisionField[1];
	}

	return DivisionField[0];

}

int SurfaceTriangle::GetTagCtrl234(int ctrl_index) {
	// illegal index variable
	if(ctrl_index <= 1 || ctrl_index > MaxIntCtrl)
		return -1;

	int * control = this->GetTagCtrl234Pointer(ctrl_index);

	return *control;

}


void SurfaceTriangle::FloodTagCtrl234(int ctrl_index, int tag) {
	// illegal index variable
	if(ctrl_index <= 1 || ctrl_index > MaxIntCtrl)
		return;

	int *control = this->GetTagCtrl234Pointer(ctrl_index);

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
	this->WaveTagCtrl234Internal(ctrl_index, tag);
	return this->GetMaxTagCtrl234(ctrl_index);
}

int SurfaceTriangle::WaveTagCtrl234Internal(int ctrl_index, int tag, bool prvni) {
	// return maximum tag on the surface

	// illegal index variable
	if(ctrl_index <= 1 || ctrl_index > MaxIntCtrl)
		return tag-20;

	int *control = this->GetTagCtrl234Pointer(ctrl_index);
	const long int almost_infinity = ALMOST_INFINITY;

	if(prvni) { // first call fills surface by almost infinity
		this->FloodTagCtrl234(ctrl_index, almost_infinity);
		*control = tag;
	} else {
		if(*control <= tag) {
			return tag;
		} else {
			*control = tag;
		}
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

	int *control = this->GetTagCtrl234Pointer(ctrl_index),
			*lead = this->GetTagCtrl234Pointer(lead_index);
	const long int almost_infinity = ALMOST_INFINITY;

	if(cameFrom == NULL) { // first call fills surface by almost infinity
		this->FloodTagCtrl234(ctrl_index, almost_infinity);
		*control = tag;
		minVal = *lead -1;
		maxVal = *lead + 1;
		//printf("   zacatek %d %d, %d %d, %d -- %d %d\n", minVal, maxVal, _intControl2, _intControl3, _id, ctrl_index, lead_index);
	} else {
		if(*control <= tag || *lead < minVal || *lead > maxVal) {
			return this;
		} else {
			*control = tag;
			//printf("   %d %d, %d %d, %d -- %d %d\n", minVal, maxVal, _intControl2, _intControl3, _id, ctrl_index, lead_index);
		}
	}

	SurfaceTriangle ** point = new SurfaceTriangle *[3];
	SurfaceTriangle * ret = NULL;

	point[0] = this; point[1] = this; point[2] =this;

	// flood subdivide
	for(int i = 0; i <3; i++) {
		//printf("        soused %d, pan %d: %d %d\n", _id, AllAround[i]->GetID(), AllAround[i]->GetTagCtrl234(lead_index), AllAround[i]->GetTagCtrl234(ctrl_index));
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

	int *control = this->GetTagCtrl234Pointer(ctrl_index);
	const long int almost_infinity = ALMOST_INFINITY;

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

SurfaceTriangle * SurfaceTriangle::FindMinSquareValue(int ctrl_index1, int ctrl_index2, int tag1, int tag2) {
	this->FloodTagCtrl1(SURFACE_CONTROL_TAG1);
	this->FindMinSquareValueInternal(ctrl_index1, ctrl_index2, tag1, tag2);
}

SurfaceTriangle * SurfaceTriangle::FindMinSquareValueInternal(int ctrl_index1, int ctrl_index2, int tag1, int tag2) {
	if(_intControl1 != SURFACE_CONTROL_TAG1)
		return NULL;

	_intControl1 = SURFACE_CONTROL_CLEAN;

	// illegal index variable
	if(ctrl_index1 <= 1 || ctrl_index1 > MaxIntCtrl)
		return NULL;
	if(ctrl_index2 <= 1 || ctrl_index2 > MaxIntCtrl)
		return NULL;

	int *ctrl1 = this->GetTagCtrl234Pointer(ctrl_index1);
	int *ctrl2 = this->GetTagCtrl234Pointer(ctrl_index2);

	SurfaceTriangle * Pa1 = AllAround[0]->FindMinSquareValueInternal(ctrl_index1, ctrl_index2, tag1, tag2);
	SurfaceTriangle * Pa2 = AllAround[1]->FindMinSquareValueInternal(ctrl_index1, ctrl_index2, tag1, tag2);
	SurfaceTriangle * Pa3 = AllAround[2]->FindMinSquareValueInternal(ctrl_index1, ctrl_index2, tag1, tag2);

	double a1 = ALMOST_INFINITY, a2 = ALMOST_INFINITY, a3 = ALMOST_INFINITY, thisVal;
	if(Pa1 != NULL)
		a1 = (Pa1->GetTagCtrl234(ctrl_index1) - tag1)*(Pa1->GetTagCtrl234(ctrl_index1) - tag1)
				+ (Pa1->GetTagCtrl234(ctrl_index2) - tag2)*(Pa1->GetTagCtrl234(ctrl_index2) - tag2);
	if(Pa2 != NULL)
		a2 = (Pa2->GetTagCtrl234(ctrl_index1) - tag1)*(Pa2->GetTagCtrl234(ctrl_index1) - tag1)
				+ (Pa2->GetTagCtrl234(ctrl_index2) - tag2)*(Pa2->GetTagCtrl234(ctrl_index2) - tag2);
	if(Pa3 != NULL)
		a3 = (Pa3->GetTagCtrl234(ctrl_index1) - tag1)*(Pa3->GetTagCtrl234(ctrl_index1) - tag1)
				+ (Pa3->GetTagCtrl234(ctrl_index2) - tag2)*(Pa3->GetTagCtrl234(ctrl_index2) - tag2);
		thisVal = (*ctrl1 - tag1)*(*ctrl1 - tag1) + (*ctrl2 - tag2)*(*ctrl2 - tag2);

		if(thisVal <= a1 && thisVal <= a2 && thisVal <= a3)
			return this;
		else {
			if(a1 <= a2 && a1 <= a3 && Pa1 != NULL)
				return Pa1;

			if(a2 <= a1 && a2 <= a3 && Pa2 != NULL)
						return Pa2;

			if(Pa3 != NULL)
				return Pa3;
		}

		printf("error\n");
		return this;
}

void SurfaceTriangle::PrintSurface() {
	this->FloodTagCtrl1(SURFACE_CONTROL_TAG1);
	this->PrintSurfaceInternal();
}

void SurfaceTriangle::PrintSurfaceInternal() {
	if(_intControl1 == SURFACE_CONTROL_TAG1) {
		printf("%d (%d %d %d, %.3f %.3f), %d\n",_intControl1, _intControl2, _intControl3, _intControl4,
				data->u_coordinate*180/M_PI, data->v_coordinate*180/M_PI, _id);
		printf("  soused - (%d %d %d), %d\n",AllAround[0]->GetTagCtrl234(2),AllAround[0]->GetTagCtrl234(3),AllAround[0]->GetTagCtrl234(4), AllAround[0]->GetID());
		printf("  soused - (%d %d %d), %d\n",AllAround[1]->GetTagCtrl234(2),AllAround[1]->GetTagCtrl234(3),AllAround[1]->GetTagCtrl234(4), AllAround[1]->GetID());
		printf("  soused - (%d %d %d), %d\n",AllAround[2]->GetTagCtrl234(2),AllAround[2]->GetTagCtrl234(3),AllAround[2]->GetTagCtrl234(4), AllAround[2]->GetID());
	} else return;

	_intControl1 = SURFACE_CONTROL_CLEAN;

	AllAround[0]->PrintSurfaceInternal();
	AllAround[1]->PrintSurfaceInternal();
	AllAround[2]->PrintSurfaceInternal();
}

int SurfaceTriangle::GetMaxTagCtrl234(int ctrl_index) {
	this->FloodTagCtrl1(SURFACE_CONTROL_TAG1);
	this->GetMaxTagCtrl234Internal(ctrl_index);
}

int SurfaceTriangle::GetMaxTagCtrl234Internal(int ctrl_index) {
	if(_intControl1 != SURFACE_CONTROL_TAG1)
		return -ALMOST_INFINITY;

	_intControl1 = SURFACE_CONTROL_CLEAN;

	// illegal index variable
	if(ctrl_index <= 1 || ctrl_index > MaxIntCtrl)
		return 0;

	int *control = this->GetTagCtrl234Pointer(ctrl_index);

	int a1 = AllAround[0]->GetMaxTagCtrl234Internal(ctrl_index);
	int a2 = AllAround[1]->GetMaxTagCtrl234Internal(ctrl_index);
	int a3 =AllAround[2]->GetMaxTagCtrl234Internal(ctrl_index);

	a1 = a1 >= *control ? a1 : *control;

	return a1 > a2 ? (a1 > a3 ? a1 : a3) : (a2 > a3 ? a2 : a3);
}

void SurfaceTriangle::CreateD20() {
	// create D20
	SurfaceTriangle * d01 = this,
	*d02 = new SurfaceTriangle(),
	*d03 = new SurfaceTriangle(),
	*d04 = new SurfaceTriangle(),
	*d05 = new SurfaceTriangle(),
	*d06 = new SurfaceTriangle(),
	*d07 = new SurfaceTriangle(),
	*d08 = new SurfaceTriangle(),
	*d09 = new SurfaceTriangle(),
	*d10 = new SurfaceTriangle(),
	*d11 = new SurfaceTriangle(),
	*d12 = new SurfaceTriangle(),
	*d13 = new SurfaceTriangle(),
	*d14 = new SurfaceTriangle(),
	*d15 = new SurfaceTriangle(),
	*d16 = new SurfaceTriangle(),
	*d17 = new SurfaceTriangle(),
	*d18 = new SurfaceTriangle(),
	*d19 = new SurfaceTriangle(),
	*d20 = new SurfaceTriangle();

	d01->SetAround(0, d07);
	d01->SetAround(1, d19);
	d01->SetAround(2, d13);

	d02->SetAround(0, d18);
	d02->SetAround(1, d20);
	d02->SetAround(2, d12);

	d03->SetAround(0, d17);
	d03->SetAround(1, d16);
	d03->SetAround(2, d19);

	d04->SetAround(0, d14);
	d04->SetAround(1, d18);
	d04->SetAround(2, d11);

	d05->SetAround(0, d15);
	d05->SetAround(1, d13);
	d05->SetAround(2, d18);

	d06->SetAround(0, d14);
	d06->SetAround(1, d09);
	d06->SetAround(2, d16);

	d07->SetAround(0, d01);
	d07->SetAround(1, d15);
	d07->SetAround(2, d17);

	d08->SetAround(0, d16);
	d08->SetAround(1, d10);
	d08->SetAround(2, d20);

	d09->SetAround(0, d06);
	d09->SetAround(1, d11);
	d09->SetAround(2, d19);

	d10->SetAround(0, d12);
	d10->SetAround(1, d08);
	d10->SetAround(2, d17);

	d11->SetAround(0, d13);
	d11->SetAround(1, d09);
	d11->SetAround(2, d04);

	d12->SetAround(0, d15);
	d12->SetAround(1, d02);
	d12->SetAround(2, d10);

	d13->SetAround(0, d05);
	d13->SetAround(1, d01);
	d13->SetAround(2, d11);

	d14->SetAround(0, d06);
	d14->SetAround(1, d20);
	d14->SetAround(2, d04);

	d15->SetAround(0, d12);
	d15->SetAround(1, d07);
	d15->SetAround(2, d05);

	d16->SetAround(0, d03);
	d16->SetAround(1, d08);
	d16->SetAround(2, d06);

	d17->SetAround(0, d10);
	d17->SetAround(1, d03);
	d17->SetAround(2, d07);

	d18->SetAround(0, d04);
	d18->SetAround(1, d02);
	d18->SetAround(2, d05);

	d19->SetAround(0, d01);
	d19->SetAround(1, d03);
	d19->SetAround(2, d09);

	d20->SetAround(0, d02);
	d20->SetAround(1, d14);
	d20->SetAround(2, d08);
}

void SurfaceTriangle::CreateD4() {
		// 1d4 for testing
		SurfaceTriangle *triangle1 = this;

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

}

int* SurfaceTriangle::GetTagCtrl234Pointer(int ctrl_index) {
	int *control = NULL;

	switch(ctrl_index) {
	case 2:
		control = &_intControl2;
		break;
	case 3:
		control = &_intControl3;
		break;
	case 4:
		control = &_intControl4;
		break;
	default:
		throw "shouldn't happen in GetTagCtrl234Pointer";
		return NULL;
	}

	return control;
}

void SurfaceTriangle::CreateSphericalCoordinates() {
	// point at which this is called will be the north pole

	this->NullControls();
	int out1 = this->WaveTagCtrl234(2, 0);
	SurfaceTriangle * bodRovniku = this->FindFirstTagValue(2, out1/2);

	if(bodRovniku == NULL)
		return;

	int out2 = bodRovniku->WaveTagCtrl234(3, 0);
	SurfaceTriangle * bod2Rovniku = this->FindMinSquareValue(2,3, out1/2, out2/2);
	int out3 = bod2Rovniku->WaveTagCtrl234(4, 0);

	this->FloodTagCtrl1(SURFACE_CONTROL_TAG1);
	this->CreateSphericalCoordinatesInternal(out1, out2, out3);
}

void SurfaceTriangle::CreateSphericalCoordinatesInternal(int max2, int max3, int max4) {
	if(_intControl1 != SURFACE_CONTROL_TAG1)
		return;

	_intControl1 = SURFACE_CONTROL_CLEAN;

	this->data->u_coordinate = (_intControl2 - max2/2.0)/max2 * M_PI;

	double
		x = sin((_intControl3 - max3/2.0)/max3 * M_PI),
		y = sin((_intControl4 - max4/2.0)/max4 * M_PI);

	this->data->v_coordinate = atan2(x, y);

	AllAround[0]->CreateSphericalCoordinatesInternal(max2, max3, max4);
	AllAround[1]->CreateSphericalCoordinatesInternal(max2, max3, max4);
	AllAround[2]->CreateSphericalCoordinatesInternal(max2, max3, max4);
}
