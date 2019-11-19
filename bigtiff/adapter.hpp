#pragma once

#include <cstdint>
#include <vector>
#include "bimtypes.hpp"
#include "spatialmap.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/core/core.hpp"

using namespace bim;

struct METADATA {
	/*
	 * METADATA - Structure that stores the metadata information of a TIFF
	 *            bigimage. This metadata information is read from TIFF tags.
	 */

	const SIZE   TileSize = {0, 0};
	const UINT64 RowsPerStrip = 0;
	const UINT64 Height = 0;
	const UINT64 Width = 0;
	const UINT8  BitsPerSample = 0;
	const UINT8  Channels = 0;
	const UINT8  PlanarConfiguration = 0;
	const UINT8  Photometric = 0;
	const UINT8  SampleFormat = 0;
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
		cv::Mat readPatch(UINT16 level, INDEX pindex, SIZE patchSize);
		cv::Mat readRegion(UINT16 level, LOCATION regStart, LOCATION regEnd);

	private:
		void readMetadata();
		cv::Mat readIOTile(UINT16 level, INDEX origin);
		vector<INDEX> extents(UINT16 level, SIZE unitsize, INDEX pindex);
		UINT8 getDatatype(UINT16 level);
};
