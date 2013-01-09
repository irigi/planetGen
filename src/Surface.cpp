#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "Surface.h"
#include "PhysicalData.h"

long SurfaceTriangle::_id_counter = 0;
long SurfaceTriangle::_debug = 0;

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
	_doubleControl = 0;
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
	if(DEBUG_NULL) _debug = 0;
	this->NullControlsInternal(SURFACE_CONTROL_INTERNAL, true);
	this->NullControlsInternal(SURFACE_CONTROL_CLEAN, true);
	if(DEBUG_NULL) printf("NullControls: recursion called %ld\n", _debug);
}

void SurfaceTriangle::FloodTagCtrl1(int tag) {
	if(SURFACE_CONTROL_INTERNAL == tag) {
		throw "Internal tag musn't be used in SurfaceTriangle::FloodTag!";
		return;
	}

	this->NullControlsInternal(tag);
}

void SurfaceTriangle::NullControlsInternal(const int stage, bool delete_234controls) {
	if(DEBUG_NULL) _debug++;
	_intControl1 = stage;

	if(delete_234controls) {
		// these do not control null algorithm
		_intControl2 = 0;
		_intControl3 = 0;
		_intControl4 = 0;
		_doubleControl = 0;
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
	int ret = 0, max = 100;
	if(DEBUG_WAVE) _debug = 0;

	// this prevents almost infinite diving into the recursion while preserving the generality
	this->WaveTagCtrl234Internal(ctrl_index, tag, max);
	while((ret=this->GetMaxTagCtrl234(ctrl_index)) == ALMOST_INFINITY) {
		max *= 2;
		this->WaveTagCtrl234Internal(ctrl_index, tag, max);
	}
	if(DEBUG_WAVE) printf("WaveTagCtrl234: recursion calls %ld\n", _debug);
	return ret;
}

void SurfaceTriangle::WaveTagCtrl234Internal(int ctrl_index, int tag, int max, bool prvni) {
	// return maximum tag on the surface
	if(DEBUG_WAVE) _debug++;

	// illegal index variable
	if(ctrl_index <= 1 || ctrl_index > MaxIntCtrl)
		return;

	int *control = this->GetTagCtrl234Pointer(ctrl_index);

	if(control == NULL)
		return;

	if(prvni) { // first call fills surface by almost infinity
		this->FloodTagCtrl234(ctrl_index, ALMOST_INFINITY);
		*control = tag;
	} else {
		if(*control <= tag || tag >= max) {
			return;
		} else {
			*control = tag;
		}
	}

	// flood subdivide
	AllAround[0]->WaveTagCtrl234Internal(ctrl_index, tag+1, max, false);
	AllAround[1]->WaveTagCtrl234Internal(ctrl_index, tag+1, max, false);
	AllAround[2]->WaveTagCtrl234Internal(ctrl_index, tag+1, max, false);

	return;
}

SurfaceTriangle * SurfaceTriangle::FindFirstTagValue(int ctrl_index, int tag) {
	// illegal index variable
	if(ctrl_index <= 1 || ctrl_index > MaxIntCtrl)
		return NULL;

	int *control = this->GetTagCtrl234Pointer(ctrl_index);

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
	return this->FindMinSquareValueInternal(ctrl_index1, ctrl_index2, tag1, tag2);
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
		printf("%d (%d %d %d %f, %.3f %.3f), %d\n",_intControl1, _intControl2, _intControl3, _intControl4, _doubleControl,
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
	return this->GetMaxTagCtrl234Internal(ctrl_index);
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

	SurfaceTriangle ** array = NULL;

	int i = 1;
	this->WaveDoubleFromTag(2, 0);
	array = this->GetAllCellsWithGivenTag(2,1);
	while(array != NULL) {
		SortAccordingToDoubleControl(array);

		int j = 0;
		if(array != NULL) {
			while(array[j] != NULL) {
				//printf("..%d (%f)",array[j]->GetID(), array[j]->GetDoubleTag());
				j++;
			}
			//printf("\n");
		}

		for(int k = 0; k < j; k++) {
			array[k]->SetDoubleTag(double(k)/j*M_PI*2);
		}

		//j = 0;
		//if(array != NULL) {
		//	while(array[j] != NULL) {
		//		printf("..%d (%f)",array[j]->GetID(), array[j]->GetDoubleTag());
		//		j++;
		//	}
		//	printf("\n");
		//}

		this->WaveDoubleFromTag(2, i);

		// delete old array and create new one
		SurfaceTriangle ** tmp = array;
		array = this->GetAllNeighborsWithGivenTag(2,i+1,array);
		delete [] tmp; tmp = NULL;

		if(array[0] == NULL) {
			//printf("deleting empty array\n");
			delete [] array;
			array = NULL;
		}

		i++;
	}

	this->FloodTagCtrl1(SURFACE_CONTROL_TAG1);
	this->CreateSphericalCoordinatesInternal(out1);
}

void SurfaceTriangle::CreateSphericalCoordinatesInternal(int max2) {
	if(_intControl1 != SURFACE_CONTROL_TAG1)
		return;

	_intControl1 = SURFACE_CONTROL_CLEAN;

	this->data->u_coordinate = (_intControl2 - max2/2.0)/max2 * M_PI;

	this->data->v_coordinate = _doubleControl;

	AllAround[0]->CreateSphericalCoordinatesInternal(max2);
	AllAround[1]->CreateSphericalCoordinatesInternal(max2);
	AllAround[2]->CreateSphericalCoordinatesInternal(max2);
}

SurfaceTriangle ** SurfaceTriangle::GetAllNeighborsWithGivenTag(int ctrl_index, int tag, SurfaceTriangle ** array) {
	if(array == NULL)
		return NULL;

	if(ctrl_index == 4) {
		printf("Cannot use index 4 in GetAllNeighborsWithGivenTag, it is used internaly\n");
		return NULL;
	}

	int i = 0, cells_to_return = 0;
	while(array[i] != NULL) {
		if(array[i]->AllAround[0]->GetTagCtrl234(ctrl_index) == tag) {
			cells_to_return++;
			*(array[i]->AllAround[0]->GetTagCtrl234Pointer(4)) = 1; // this causes each element is only once in the result array
		}
		if(array[i]->AllAround[1]->GetTagCtrl234(ctrl_index) == tag) {
			cells_to_return++;
			*(array[i]->AllAround[1]->GetTagCtrl234Pointer(4)) = 1;
		}
		if(array[i]->AllAround[2]->GetTagCtrl234(ctrl_index) == tag) {
			*(array[i]->AllAround[2]->GetTagCtrl234Pointer(4)) = 1;
			cells_to_return++;
		}
		i++;
	}

	SurfaceTriangle ** ret = new SurfaceTriangle *[cells_to_return+1];
	for(int i = 0; i < cells_to_return+1; i++)
		ret[i] = NULL;

	i = 0;
	int n = 0;
	while(array[i] != NULL) {
		if(array[i]->AllAround[0]->GetTagCtrl234(ctrl_index) == tag &&
				array[i]->AllAround[0]->GetTagCtrl234(4) == 1) {
			ret[n] = array[i]->AllAround[0];
			n++;
			*(array[i]->AllAround[0]->GetTagCtrl234Pointer(4)) = 0;
		}

		if(array[i]->AllAround[1]->GetTagCtrl234(ctrl_index) == tag &&
				array[i]->AllAround[1]->GetTagCtrl234(4) == 1) {
			ret[n] = array[i]->AllAround[1];
			n++;
			*(array[i]->AllAround[1]->GetTagCtrl234Pointer(4)) = 0;
		}

		if(array[i]->AllAround[2]->GetTagCtrl234(ctrl_index) == tag &&
				array[i]->AllAround[2]->GetTagCtrl234(4) == 1) {
			ret[n] = array[i]->AllAround[2];
			n++;
			*(array[i]->AllAround[2]->GetTagCtrl234Pointer(4)) = 0;
		}

		i++;
	}


	return ret;
}

SurfaceTriangle ** SurfaceTriangle::GetAllCellsWithGivenTag(int ctrl_index, int tag) {
	this->FloodTagCtrl1(SURFACE_CONTROL_TAG1);
	return this->GetAllCellsWithGivenTagInternal(ctrl_index, tag);
}

SurfaceTriangle ** SurfaceTriangle::GetAllCellsWithGivenTagInternal(int ctrl_index, int tag) {
	if(_intControl1 != SURFACE_CONTROL_TAG1)
		return NULL;

	_intControl1 = SURFACE_CONTROL_CLEAN;

	// illegal index variable
	if(ctrl_index <= 1 || ctrl_index > MaxIntCtrl)
		return 0;

	SurfaceTriangle **  a1 = NULL, **a2 = NULL, **a3 = NULL;

	a1 = AllAround[0]->GetAllCellsWithGivenTagInternal(ctrl_index, tag);
	a2 = AllAround[1]->GetAllCellsWithGivenTagInternal(ctrl_index, tag);
	a3 = AllAround[2]->GetAllCellsWithGivenTagInternal(ctrl_index, tag);

	// we have nothing to pass
	if(a1 == NULL && a2 == NULL && a3 == NULL && this->GetTagCtrl234(ctrl_index) != tag)
		return NULL;

	// find out how much we have to pass
	int allocation = 0, i;
	if(a1 != NULL) {
			i = 0;
			while(a1[i] != NULL) {
				allocation++;
				i++;
		}
	}
	if(a2 != NULL) {
			i = 0;
			while(a2[i] != NULL) {
				allocation++;
				i++;
			}
	}
	if(a3 != NULL) {
			i = 0;
			while(a3[i] != NULL) {
				allocation++;
				i++;
			}
	}

	if(tag == this->GetTagCtrl234(ctrl_index))
		allocation++;

	// +10 is foolproof constant
	SurfaceTriangle **  ret = new SurfaceTriangle *[allocation+10];
	for(int i = 0; i < allocation + 10; i++) {
		ret[i] = NULL;
	}

	// copy everything we have
	int n = 0;
	if(a1 != NULL) {
			i = 0;
			while(a1[i] != NULL) {
				ret[n] = a1[i];
				i++; n++;
		}
	}
	if(a2 != NULL) {
			i = 0;
			while(a2[i] != NULL) {
				ret[n] = a2[i];
				i++; n++;
			}
	}
	if(a3 != NULL) {
			i = 0;
			while(a3[i] != NULL) {
				ret[n] = a3[i];
				i++; n++;
			}
	}

	if(tag == this->GetTagCtrl234(ctrl_index)) {
		ret[n] = this;
		n++;
	}

	// delete child arrays
	if(a1 != NULL) delete [] a1; a1 = NULL;
	if(a2 != NULL) delete [] a2; a2 = NULL;
	if(a3 != NULL) delete [] a3; a3 = NULL;

	return ret;
}

SurfaceTriangle ** SurfaceTriangle::SortAccordingToDoubleControl(SurfaceTriangle ** list) {
	if(list == NULL || list[0] == NULL)
		return NULL;

	bool changed = false;
	while(true) {
		int i = 1;
		while(list[i-1] != NULL && list[i] != NULL) {
			if(list[i-1]->GetDoubleTag() > list[i]->GetDoubleTag()) {
				SurfaceTriangle * tmp = list[i-1];
				list[i-1] = list[i];
				list[i] = tmp;
				changed = true;
			}

			i++;
		}

		if(!changed)
			return list;

		changed = false;
	}

	return list;
}

void SurfaceTriangle::WaveDoubleFromTag(int ctrl_index, int tag) {
	this->NullControls();
	this->FloodTagCtrl1(SURFACE_CONTROL_TAG1);
	this->WaveDoubleFromTagInternal(ctrl_index, tag);
}

void SurfaceTriangle::WaveDoubleFromTagInternal(int ctrl_index, int tag) {
	if(_intControl1 != SURFACE_CONTROL_TAG1)
		return;

	_intControl1 = SURFACE_CONTROL_CLEAN;

	if(int(true) != 1) {
		printf("WaveDoubleFromTagInternal won't work since int(true) != 1\n");
		throw "WaveDoubleFromTagInternal won't work since int(true) != 1";
		return;
	}

	// illegal index variable
	if(ctrl_index <= 1 || ctrl_index > MaxIntCtrl)
		return;

	if(tag == 0 && this->GetTagCtrl234(ctrl_index) == tag) {
		AllAround[0]->SetDoubleTag(-1);
		AllAround[1]->SetDoubleTag(0);
		AllAround[2]->SetDoubleTag(1);
	} else
	if(this->GetTagCtrl234(ctrl_index) == tag) {
		// there is only one way to spread -- no problems with orientation
		if(int(AllAround[0]->GetTagCtrl234(ctrl_index) == tag+1) +
				int(AllAround[1]->GetTagCtrl234(ctrl_index) == tag+1) +
				int(AllAround[2]->GetTagCtrl234(ctrl_index) == tag+1) == 1) {

			if(AllAround[0]->GetTagCtrl234(ctrl_index) == tag+1) {
				AllAround[0]->SetDoubleTag(_doubleControl);
			}
			if(AllAround[1]->GetTagCtrl234(ctrl_index) == tag+1) {
				AllAround[1]->SetDoubleTag(_doubleControl);
			}
			if(AllAround[2]->GetTagCtrl234(ctrl_index) == tag+1) {
				AllAround[2]->SetDoubleTag(_doubleControl);
			}
		}

		if(int(AllAround[0]->GetTagCtrl234(ctrl_index) == tag+1) +
				int(AllAround[1]->GetTagCtrl234(ctrl_index) == tag+1) +
				int(AllAround[2]->GetTagCtrl234(ctrl_index) == tag+1) == 2) {

			if(AllAround[0]->GetTagCtrl234(ctrl_index) == tag-1) {
				AllAround[1]->SetDoubleTag(_doubleControl-0.001);
				AllAround[2]->SetDoubleTag(_doubleControl+0.001);
			}
			if(AllAround[1]->GetTagCtrl234(ctrl_index) == tag-1) {
				AllAround[2]->SetDoubleTag(_doubleControl-0.001);
				AllAround[0]->SetDoubleTag(_doubleControl+0.001);
			}
			if(AllAround[2]->GetTagCtrl234(ctrl_index) == tag-1) {
				AllAround[0]->SetDoubleTag(_doubleControl-0.001);
				AllAround[1]->SetDoubleTag(_doubleControl+0.001);
			}
		}
	}

	AllAround[0]->WaveDoubleFromTagInternal(ctrl_index, tag);
	AllAround[1]->WaveDoubleFromTagInternal(ctrl_index, tag);
	AllAround[2]->WaveDoubleFromTagInternal(ctrl_index, tag);

	return;
}

void SurfaceTriangle::ExportBitmap() {

}
