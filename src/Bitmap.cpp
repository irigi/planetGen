// Copyright © 2008-2013 planetGen Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include <stdio.h>
#include <math.h>
#include "Bitmap.h"
#include "PhysicalData.h"

BitmapPixel::BitmapPixel() {
	this->data = new PhysicalData();
}
