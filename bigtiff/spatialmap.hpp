#pragma once

#include "bimtypes.hpp"
#include <array>

using namespace btiff;
using namespace std;

class BIGTIFF_DLL spatialmap {
	/*
		Spatial Mapping Properties -------------------------------------------------
	*/

	public:
		RANGE  XLimits;
		RANGE  YLimits;
		SIZE   ImageSize;
		DOUBLE XSpacing;
		DOUBLE YSpacing;

	/*
		Spatial Mapping Methods ----------------------------------------------------
	*/
	public:
		spatialmap(RANGE xlim, RANGE ylim, SIZE imsize);
		LOCATION imageToSpatial(INDEX pindex);
		INDEX spatialToImage(LOCATION coordinate);
};