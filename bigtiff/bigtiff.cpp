#include "bigtiff.hpp"
#include "opencv2/core/core.hpp"

/*
------------------------------------------------------------------------------------
	API METHODS
------------------------------------------------------------------------------------
*/

namespace {
	UINT16 ReadLevel;
	vector<LOCATION>::iterator sequentialReader;
}

// ---------------------------------------------------------------------------------
bigtiff::bigtiff(STRING file)
{
	/*
		BIGTIFF - Class to perform sequential or on-demand patch I/O from a BigTIFF
			      image file.

		// TODO
	*/

	// Create I/O adapter:
	Source  = file;
	Adapter = adapter(file, "r");
	NumDirectories = Adapter.getNumDirectories();

	// Copy over some adapter properties:
	for (int numdir_ = 0; numdir_ < NumDirectories; numdir_++)
	{
		ImageSize[numdir_] = { Adapter.MetaData[numdir_].Width,
							   Adapter.MetaData[numdir_].Height };

		Channels[numdir_] = Adapter.MetaData[numdir_].Channels;
	}

	// Construct spatial maps for each directory image:
	array<RANGE, 2> extents = spatialExtents(ImageSize);

	for (int numdir_ = 0; numdir_ < NumDirectories; numdir_++)
	{
		SpatialMapping[numdir_] = 
			spatialmap(extents[0], extents[1], ImageSize[numdir_]);
	}

	// Compute default patch sizes:
	// NOTE: PatchSize for a directory level is set to be 2*[W H] where W is the
	//		 tile width and H is tile height.
	PatchSize = defaultPatchSize(2, 2);

	// Compute patch map
	createPatchMap();

	// Default: Point the sequential reader to read from the first TIFF directory:
	this->set_ReadDirectory(0);
}

// ---------------------------------------------------------------------------------
PATCH bigtiff::getPatch(UINT16 level, LOCATION spatialCoordinate)
{
	/*
		TODO
	*/

	// Convert spatial coordinate to image index:
	INDEX pindex_ = SpatialMapping[level].spatialToImage(spatialCoordinate);

	// Compute patch image extents based on specified spatial coordinate:
	vector<INDEX> imageExtents = Adapter.extents(level, PatchSize[level], pindex_);

	//Convert patch extents to spatial extents:
	LOCATION spatialOrigin = SpatialMapping[level].imageToSpatial(imageExtents[0]);
	LOCATION spatialEnd = SpatialMapping[level].imageToSpatial(imageExtents[1]);

	// Read patch:
	Mat data_ = Adapter.readPatch(level, pindex_, PatchSize[level]);

	//Structurize the data:
	PATCH patch_ = PATCH(data_,
						 spatialOrigin,
						 spatialEnd,
						 imageExtents[0],
						 imageExtents[1]);

	return patch_;
}

// ---------------------------------------------------------------------------------
PATCH bigtiff::getRegion(UINT16 level, LOCATION regStart, LOCATION regEnd)
{

	PATCH patch_ = PATCH(Mat(), { 0, 0 }, { 120, 120 }, { 0,0 }, { 120, 120 });
	return patch_;
}

// ---------------------------------------------------------------------------------
PATCH bigtiff::seqread()
{
	/*
		Sequentially read patches from the images
	*/

	// Get next origin:
	LOCATION patchOrigin_ = (*sequentialReader);

	// Let adapter handle read:
	PATCH patch_ = getPatch(ReadLevel, patchOrigin_);

	return patch_;
}

/*
------------------------------------------------------------------------------------
	GET/SET METHODS
------------------------------------------------------------------------------------
*/

void bigtiff::set_PatchSize(SIZE patchSize) { this->PatchSize[0] = patchSize; }
void bigtiff::set_PatchSize(vector<SIZE> patchSize) { this->PatchSize = patchSize; }
void bigtiff::set_PatchStride(SIZE patchStride) { this->PatchStride[0] = patchStride; }
void bigtiff::set_PatchStride(vector<SIZE> patchStride) { this->PatchStride = patchStride; }
void bigtiff::set_CacheDir(STRING cacheDir) { this->CacheDir = cacheDir; }

void bigtiff::set_ReadDirectory(UINT16 level)
{
	/*
		TODO
	*/

	// Point the sequential iterator to the start of the patches at specified level:
	ReadLevel = level;
	sequentialReader = PatchMap.at(ReadLevel).begin();
}

/*
------------------------------------------------------------------------------------
	PRIVATE HELPER METHODS
------------------------------------------------------------------------------------
*/

//----------------------------------------------------------------------------------
vector<SIZE> bigtiff::defaultPatchSize(int m, int n)
{
	/*
		BIGTIFF method to calculate optimal patch sizes for each TIFF directory.
	*/

	vector<SIZE> patchSizes;

	for (int numdir_ = 0; numdir_ < NumDirectories; numdir_++)
	{
		// Find PatchSize for tiled images:
		if (Adapter.istiled()) 
		{
			// PatchSize is a multiple of I/O TileSize:
			patchSizes[numdir_] = Adapter.MetaData[numdir_].TileSize;
			patchSizes[numdir_][0] = patchSizes[numdir_][0] * m;
			patchSizes[numdir_][1] = patchSizes[numdir_][1] * n;
		}
		else 
		{
			// Select a constant patch size of [1024 1024].
			patchSizes[numdir_] = { 1024, 1024 };
		}
	}

	return patchSizes;
}

//----------------------------------------------------------------------------------
array<RANGE, 2> bigtiff::spatialExtents(vector<SIZE> imsizes)
{
	/*
		BIGTIFF method to find extents of the spatial coordinate system to map the 
		image. This is equivalent to the image size of the finest TIFF directory.
	*/

	array<RANGE, 2> extents;
	vector<UINT64> pxcount;

	// Find largest image:
	for (int level = 0; level < imsizes.size(); level++)
	{
		// Find the pixel count at LEVEL
		pxcount[level] = imsizes[level][0] * imsizes[level][1];
	}

	UINT16 finest_ = static_cast<UINT16>( distance(pxcount.begin(), 
							  max_element(pxcount.begin(), pxcount.end())) );

	// Define extents based on finest image size:
	// NOTE: Since we picked a 1-1 map of finest pixel and spatial units, 
	//		 the spacing in each dimension is 1.
	SIZE spatialSize_ = imsizes[finest_];
	DOUBLE px_ = 1, py_ = 1;

	extents[0] = { 0 - px_ / 2, static_cast<DOUBLE>(spatialSize_[0]) - px_ / 2 };
	extents[1] = { 0 - py_ / 2, static_cast<DOUBLE>(spatialSize_[1]) - py_ / 2 };

	return extents;
}

//----------------------------------------------------------------------------------
void bigtiff::createPatchMap()
{
	/*
		createPatchMap - Creates a hash map of how image directories are broken down
		during I/O.
	*/

	for (int ndir_ = 0; ndir_ < NumDirectories; ndir_++)
	{
		RANGE xlim_ = SpatialMapping[ndir_].XLimits;
		RANGE ylim_ = SpatialMapping[ndir_].YLimits;
		INDEX patchInd_ = {0, 0};
		LOCATION patchOrigin = SpatialMapping[ndir_].imageToSpatial(patchInd_);

		do {
			// Add patch origin in spatial coordinates:
			PatchMap[ndir_].push_back(patchOrigin);

			// Increment the origin:
			DOUBLE px_ = SpatialMapping[ndir_].XSpacing;
			DOUBLE py_ = SpatialMapping[ndir_].XSpacing;

			patchOrigin[0] = patchOrigin[0] + px_*PatchStride[ndir_][0];
			patchOrigin[1] = patchOrigin[1] + py_*PatchStride[ndir_][1];

		} while (static_cast<DOUBLE>(patchOrigin[0]) <= xlim_[1] ||
				 static_cast<DOUBLE>(patchOrigin[1]) <= ylim_[1]);
	}

}