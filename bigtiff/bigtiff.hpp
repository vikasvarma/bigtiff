#pragma once

#include "bimtypes.hpp"
#include "adapter.hpp"
#include "spatialmap.hpp"
#include <map>
#include <vector>

using namespace std;

struct PATCH {
	/*
		PATCH - Structure containing Patch information.
	*/

	Mat		 Data;
	SIZE	 PatchSize;
	LOCATION SpatialOrigin;
	LOCATION SpatialEnd;
	INDEX	 ImageOrigin;
	INDEX	 ImageEnd;

	PATCH(Mat data_,
		  LOCATION spatialOrigin_,
		  LOCATION spatialEnd_,
		  INDEX imageOrigin_,
		  INDEX imageEnd_)
	{
		Data          = data_;
		SpatialOrigin = spatialOrigin_;
		SpatialEnd    = spatialEnd_;
		ImageOrigin   = imageEnd_;
		PatchSize     = { static_cast<UINT64>(data_.cols),
						  static_cast<UINT64>(data_.rows) };
	}
};

class bigtiff {
	/*
		BIGTIFF - Class to read patches of data from a BigTIFF file.

		// TODO
	*/

	public:
		STRING         Source;
		adapter        Adapter;
		vector<SIZE>   ImageSize;
		UINT16	       NumDirectories;
		vector<SIZE>   PatchSize;
		vector<SIZE>   PatchStride;
		STRING         CacheDir;
		vector<STRING> Datatype;
		vector<UINT8>  Channels;
		STRING         ColorSpace;
		vector<spatialmap> SpatialMapping;

		map<UINT16, vector<LOCATION>> PatchMap;

	public:
		bigtiff(STRING file);
		PATCH getPatch(UINT16 level, LOCATION spatialCoordinate);
		PATCH getRegion(UINT16 level, LOCATION regStart, LOCATION regEnd);
		PATCH seqread();

		// Property setters:
		void set_PatchSize(SIZE patchSize);
		void set_PatchSize(vector<SIZE> patchSize);
		void set_PatchStride(SIZE patchStride);
		void set_PatchStride(vector<SIZE> patchStride);
		void set_CacheDir(STRING cacheDir);
		void set_ReadDirectory(UINT16 level);

	private:
		vector<SIZE> defaultPatchSize(int m, int n);
		array<RANGE, 2> spatialExtents(vector<SIZE> imsizes);
		void createPatchMap();
};