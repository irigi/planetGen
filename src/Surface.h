// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#define SURFACE_CONTROL_CLEAN	0
#define SURFACE_CONTROL_TAG1	1
#define SURFACE_CONTROL_TAG2	2
#define SURFACE_CONTROL_INTERNAL	42

class SurfaceTriangle {
public:
	SurfaceTriangle();
	SurfaceTriangle * Around(int index);
	void SetAround(int index, SurfaceTriangle * target);
	void SubdivideThis();
	void NullControls();

	double temperature, rainfall;
	int IntControl1(void) {return _intControl1; }

private:
	void NullControlsInternal(const int stage);
	SurfaceTriangle ** AllAround;
	int _intControl1;
};

class SurfaceSphere : SurfaceTriangle {
public:
	SurfaceSphere();
};

class SurfaceToroid : SurfaceTriangle {

};

class SurfacePlanet : SurfaceTriangle {

};
