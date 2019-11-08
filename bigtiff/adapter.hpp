#pragma once

#include <cstdint>
#include "bimtypes.hpp"
#include "opencv2/core/mat.hpp"

struct METADATA {
	/*
	 * METADATA - Structure that stores the metadata information of a TIFF
	 *            bigimage. This metadata information is read from TIFF tags.
	 */

	const bim::SIZE   TileSize = {0, 0};
	const bim::DOUBLE RowsPerStrip = 0;
	const bim::DOUBLE Height = 0;
	const bim::DOUBLE Width = 0;
	const bim::DOUBLE BitsPerSample = 0;
	const bim::DOUBLE Channels = 0;
	const bim::UINT8  PlanarConfiguration = 0;
	const bim::DOUBLE Photometric = 0;
	const bim::UINT8  SampleFormat = 0;
	const bim::DOUBLE Colormap = 0;

};

class BIGTIFF_DLL adapter {

	/*
	 * Adapter Properties
	 */
public:
	// Adapter Properties:
	bim::STRING TIFFSource;
	bim::STRING Mode;
	bim::UINT8  NumDirectories;
	METADATA* MetaData;

private:
	void* tif;


	/*
	 * Adapter methods
	 */
public:
	adapter(bim::STRING file, bim::STRING mode);
	~adapter();
	cv::Mat readPatch(bim::UINT16 level, bim::LOCATION origin, bim::SIZE blockSize);

private:
	void readMetadata();
};
