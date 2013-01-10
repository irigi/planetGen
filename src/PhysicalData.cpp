// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include <stdio.h>
#include <math.h>
#include "PhysicalData.h"

PhysicalData::PhysicalData() {
	temperature = 0;
	rainfall = 0;
	solarConstant = 0;

	u_coordinate = 0;
	v_coordinate = 0;
}

PhysicalData::PhysicalData(PhysicalData * data) {
	temperature = data->temperature;
	rainfall = data->rainfall;
	solarConstant = data->solarConstant;

	u_coordinate = data->u_coordinate;
	v_coordinate = data->v_coordinate;
}

float PhysicalData::ValueToStore(int valueCode) {
	switch(valueCode) {
	case CODE_TEMPERATURE:
		return temperature;
		break;
	case CODE_RAINFALL:
		return rainfall;
		break;
	case CODE_SOLCONST:
		return solarConstant;
		break;
	case CODE_UCOORD:
		return u_coordinate;
		break;
	case CODE_VCOORD:
		return v_coordinate;
		break;
	}

	return 0;
}
