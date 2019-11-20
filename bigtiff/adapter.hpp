#pragma once

#include <cstdint>
#include <vector>
#include "bimtypes.hpp"
#include "spatialmap.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/core/core.hpp"

using namespace cv;
using namespace btiff;

struct METADATA {
	/*
		METADATA - Structure that stores the metadata information of a BigTIFF
		           image. This metadata information is read from TIFF tags.
	 */

	const SIZE   TileSize = {0, 0};
	const UINT64 RowsPerStrip = 0;
	const UINT64 Height = 0;
	const UINT64 Width = 0;
	const UINT16 BitsPerSample = 0;
	const UINT16 Channels = 0;
	const UINT16 PlanarConfiguration = 0;
	const UINT16 Photometric = 0;
	const UINT16 SampleFormat = 0;
	const DOUBLE Colormap = 0;

};

class BIGTIFF_DLL adapter {
	/*
		Adapter Properties ---------------------------------------------------------
	*/

	public:
		STRING    TIFFSource;
		STRING    Mode;
		UINT8     NumDirectories;
		METADATA* MetaData;
		STRING    CacheDir;

	private:
		UINT16 NumDirectories_;

	/*
		Adapter Methods ------------------------------------------------------------
	 */

	public:
		adapter();
		adapter(STRING file, STRING mode);
		~adapter();
		UINT16 getNumDirectories() const;
		bool istiled() const;
		bool isstriped() const;
		Mat readPatch(UINT16 level, INDEX pindex, SIZE patchSize);
		Mat readRegion(UINT16 level, LOCATION regStart, LOCATION regEnd);
		vector<INDEX> extents(UINT16 level, SIZE unitsize, INDEX pindex);

	private:
		void readMetadata();
		Mat readIOTile(UINT16 level, INDEX origin);
		UINT8 getDatatype(UINT16 level);
};
