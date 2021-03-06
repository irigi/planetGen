// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#define SURFACE_CONTROL_CLEAN	0
#define SURFACE_CONTROL_TAG1	1
#define SURFACE_CONTROL_TAG2	2
#define SURFACE_CONTROL_INTERNAL	-42

#define DEBUG_WAVE false
#define DEBUG_NULL false

class PhysicalData;
class Bitmap;
class BitmapPixel;

class SurfaceTriangle {
public:
	SurfaceTriangle();
	~SurfaceTriangle();

	SurfaceTriangle * Around(int index);
	PhysicalData * data;

	static const int MaxIntCtrl = 4;		// how many int ctrl variables we have (ctrl 1 is reserved)

	void SetAround(int index, SurfaceTriangle * target);
	void SetDivideImunity(bool value) { _divideImunity = value; };

	SurfaceTriangle * SubdivideSurface();	// subdivides whole surface, returns one of the children of the caller cell of the new surface
	void NullControls();								// nulls all ctrl variables

	void FloodTagCtrl234(int ctrl_index, int tag);    	// floods tag value into ctrl of given index
	int WaveTagCtrl234(int ctrl_index, int tag);		 	// floods tag value into ctrl of given index, it increases the value as it goes

	SurfaceTriangle * FindFirstTagValue(int ctrl_index, int tag); // return some point with given tag value, if not found, return NULL
	SurfaceTriangle * FindMinSquareValue(int ctrl_index1, int ctrl_index2, int tag1, int tag2); // return some point with given tag value, if not found, return NULL
	SurfaceTriangle * FindTrulyRandomPoint();

	void WaveDoubleFromTag(int ctrl_index, int tag);
	void SpreadDoubleFrom(int ctrl_index, int tag);

	int GetTagCtrl234(int ctrl_index);							// returns value of the ctrl variable
	int GetMaxTagCtrl234(int ctrl_index);					// returns max value of the ctrl variable on the surface
	int GetID() { return _id; }
	double GetDoubleTag() { return _doubleControl; }
	void SetDoubleTag(double setto) { _doubleControl = setto; }
	SurfaceTriangle ** GetAllCellsWithGivenTag(int ctrl_index, int tag);
	SurfaceTriangle ** GetAllNeighborsWithGivenTag(int ctrl_index, int tag, SurfaceTriangle ** array);
	static SurfaceTriangle ** SortAccordingToDoubleControl(SurfaceTriangle ** list);

	void PrintSurface();

	int IntControl1(void) {return _intControl1; }

	void CreateD20();
	void CreateD4();

	void CreateSphericalCoordinates();

	void ExportSurfaceToBitmap(Bitmap * bitmap, int  valueCode);

private:
	void NullControlsInternal(const int stage, bool delete_234controls = false);
	SurfaceTriangle * SubdivideSurfaceInternal(SurfaceTriangle *caller, SurfaceTriangle ** soused1, SurfaceTriangle ** soused2, int tag);
	void FloodTagCtrl1(int tag);									// floods tag-value from given triangle, stores it in ctrl1
	void WaveTagCtrl234Internal(int ctrl_index, int tag, int max, bool first = true);
	void PrintSurfaceInternal();
	int GetMaxTagCtrl234Internal(int ctrl_index);
	SurfaceTriangle * FindMinSquareValueInternal(int ctrl_index1, int ctrl_index2, int tag1, int tag2);
	void CreateSphericalCoordinatesInternal(int max2);
	SurfaceTriangle ** GetAllCellsWithGivenTagInternal(int ctrl_index, int tag);
	void WaveDoubleFromTagInternal(int ctrl_index, int tag);
	void ExportSurfaceToBitmapInternal(Bitmap * bitmap, int  valueCode);
	int* GetTagCtrl234Pointer(int ctrl_index);			// returns address of the ctrl variable

	SurfaceTriangle ** AllAround;									// neighbors of the cell
	SurfaceTriangle ** DivisionField;								// this is used by subdivision algorithm
	int _intControl1, _intControl2, _intControl3, _intControl4, _id;
	double _doubleControl;
	bool _divideImunity;												// if (divideImunity), the cell doesn't divide

	static long int _id_counter;											// cell counter
	static long int _debug;
};
