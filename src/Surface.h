// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

class SurfaceTriangle {
public:
	SurfaceTriangle();
	SurfaceTriangle * Around(int index);
	void SetAround(int index, SurfaceTriangle * target);
	void Subdivide();

private:
	SurfaceTriangle ** AllAround;
	double temperature, rainfall;
};

class Surface : SurfaceTriangle {

};

class SurfaceSphere : Surface {
public:
	SurfaceSphere();
};

class SurfaceToroid : Surface {

};

class SurfacePlanet : Surface {

};
