// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#define SURFACE_CONTROL_CLEAN	0
#define SURFACE_CONTROL_TAG1	1
#define SURFACE_CONTROL_TAG2	2
#define SURFACE_CONTROL_INTERNAL	42

class PhysicalData {
public:
	PhysicalData();
	double temperature, rainfall;
};

class SurfaceTriangle {
public:
	SurfaceTriangle();
	~SurfaceTriangle();

	SurfaceTriangle * Around(int index);
	PhysicalData * data;

	void SetAround(int index, SurfaceTriangle * target);
	void SubdivideThis();
	void SubdivideSurface();
	void NullControls();
	void FloodTagCtrl1(int tag);
	void SetDivideImunity(bool value) { _divideImunity = value; };


	int IntControl1(void) {return _intControl1; }

private:
	void NullControlsInternal(const int stage);
	void SubdivideSurfaceInternal();

	SurfaceTriangle ** AllAround;
	int _intControl1, _id;
	bool _divideImunity;

	static int _id_counter;
};

class SurfaceSphere : SurfaceTriangle {
public:
	SurfaceSphere();
};

class SurfaceToroid : SurfaceTriangle {

};

class SurfacePlanet : SurfaceTriangle {

};
