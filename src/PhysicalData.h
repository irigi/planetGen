// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#define CODE_TEMPERATURE 	1
#define CODE_RAINFALL 			2
#define CODE_SOLCONST 		3
#define CODE_UCOORD 			4
#define CODE_VCOORD 			5


class PhysicalData {
public:
	PhysicalData();
	PhysicalData(PhysicalData * data);

	float ValueToStore(int valueCode);

	float temperature, rainfall, solarConstant;
	float u_coordinate, v_coordinate;
};
