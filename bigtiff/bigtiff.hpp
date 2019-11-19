#pragma once

#include "bimtypes.hpp"
#include "adapter.hpp"
#include "spatialmap.hpp"
#include <map>
#include <vector>

using namespace std;

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
		cv::Mat getPatch(UINT16 level, LOCATION spatialCoordinate);
		cv::Mat getRegion(UINT16 level, LOCATION regStart, LOCATION regEnd);
		cv::Mat seqread();

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