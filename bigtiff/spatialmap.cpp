#include "spatialmap.hpp"
#include "math.h"
#include <algorithm>

// ---------------------------------------------------------------------------------
spatialmap::spatialmap(RANGE xlim, RANGE ylim, SIZE imsize)
{
	/*
		SMAP = spatialmap(XLIM, YLIM, IMSIZE) 
		...	Creates a spatial mapping object that is used to map image pixel 
			coordinates to spatial coordines. 
		
		Input Arguments:

		// TODO
	*/

	/*
		Input Validation -----------------------------------------------------------
	*/

	// TODO


	/*
		Initialize Class -----------------------------------------------------------
	*/

	// Define extents of the coordinate system and set class properties:
	XLimits   = xlim;
	YLimits   = ylim;
	ImageSize = imsize;

	// Find pixel extents in spatial domain:
	XSpacing  = (DOUBLE) (xlim[1] - xlim[0]) / imsize[0];
	YSpacing  = (DOUBLE) (ylim[1] - ylim[0]) / imsize[1];
}

// ---------------------------------------------------------------------------------
LOCATION spatialmap::imageToSpatial(INDEX pindex)
{
	/*
		COORDINATE = imageToSpatial(PINDEX)
		... 

		// TODO
	*/

	/*
		Algorithm ------------------------------------------------------------------
	*/

	LOCATION coordinate;

	// Find top-left corner spatial coordinate of pixel:
	coordinate[0] = XLimits[0] + pindex[0] * XSpacing;
	coordinate[1] = YLimits[0] + pindex[1] * YSpacing;

	// Spatial location of a pixel is the central spatial location:
	coordinate[0] = coordinate[0] + XSpacing / 2;
	coordinate[1] = coordinate[1] + YSpacing / 2;

	// Handle out of bounds:
	if (coordinate[0] > XLimits[1] || coordinate[0] < XLimits[0])
		coordinate[0] = std::numeric_limits<double>::quiet_NaN();
	if (coordinate[1] > YLimits[1] || coordinate[1] < YLimits[0])
		coordinate[1] = std::numeric_limits<double>::quiet_NaN();

	return coordinate;
}

// ---------------------------------------------------------------------------------
INDEX spatialmap::spatialToImage(LOCATION coordinate)
{
	/*
		INDEX = spatialToImage(IMSIZE, COORDINATE)
		...

		// TODO
	*/

	/*
		Algorithm ------------------------------------------------------------------
	*/

	INDEX pindex;

	// Subtract spatial origin, and move origin to [0 0]:
	coordinate[0] = coordinate[0] - XLimits[0];
	coordinate[1] = coordinate[1] - YLimits[0];

	// Convert spatial coordinate to pixel index:
	pindex[0] = (UINT64) std::floor( coordinate[0] / XSpacing );
	pindex[1] = (UINT64) std::floor( coordinate[1] / YSpacing );

	// Handle out of bounds:
	if (pindex[0] > ImageSize[0]) pindex[0] = -1;
	if (pindex[1] > ImageSize[1]) pindex[1] = -1;

	return pindex;
}
