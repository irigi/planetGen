// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#define SURFACE_CONTROL_CLEAN	0
#define SURFACE_CONTROL_TAG1	1
#define SURFACE_CONTROL_TAG2	2
#define SURFACE_CONTROL_INTERNAL	-42

#define ALMOST_INFINITY 999999999

class PhysicalData {
public:
	PhysicalData();
	double temperature, rainfall, solarConstant;
};

class SurfaceTriangle {
public:
	SurfaceTriangle();
	~SurfaceTriangle();

	SurfaceTriangle * Around(int index);
	PhysicalData * data;

	double u_coordinate, v_coordinate;

	void SetAround(int index, SurfaceTriangle * target);
	void SubdivideThis();					// subdivides current triangle into four
	void SubdivideSurface();				// subdivides whole surface
	void NullControls();						// nulls all ctrl variables

	void FloodTagCtrl234(int ctrl_index, int tag);    	// floods tag value into ctrl of given index
	int WaveTagCtrl234(int ctrl_index, int tag);		 	// floods tag value into ctrl of given index, it increases the value as it goes
	int FollowLineTagCtrl234(int ctrl_index, int lead_index, int tag);	// increases the ctrl variable of given index along to line
	SurfaceTriangle * FindFirstTagValue(int ctrl_index, int tag);
	int GetTagCtrl234(int ctrl_index);							// returns value of the ctrl variable
	int GetID() { return _id; }
	void PrintSurface();

	void SetDivideImunity(bool value) { _divideImunity = value; };

	static const int MaxIntCtrl = 3;		// how many int ctrl variables we have (ctrl 1 is reserved)

	int IntControl1(void) {return _intControl1; }

private:
	void NullControlsInternal(const int stage, bool delete_234controls = false);
	void SubdivideSurfaceInternal();
	void FloodTagCtrl1(int tag);									// floods tag-value from given triangle, stores it in ctrl1
	int WaveTagCtrl234Internal(int ctrl_index, int tag, bool first = true);
	SurfaceTriangle * FollowLineTagCtrl234Internal(int ctrl_index, int lead_index, int tag, SurfaceTriangle * cameFrom, int minVal, int maxVal);
	void PrintSurfaceInternal();

	SurfaceTriangle ** AllAround;
	int _intControl1, _intControl2, _intControl3, _id;
	bool _divideImunity;												// if (divideImunity), the cell doesn't divide

	static int _id_counter;											// cell counter
};

class SurfaceSphere : SurfaceTriangle {
public:
	void SphericalCoordinates();
	void CreateSphere();
	void ExportToBitmap();
};

class SurfaceToroid : SurfaceTriangle {
public:
	void ToroidalCoordinates();
	void CreateTorus();
	void ExportToBitmap();
};

class SurfaceSPK : SurfaceTriangle {
public:
	void PolarCoordinates();
	void CreateSPK();
	void ExportToBitmap();
};