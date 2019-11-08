#include "adapter.hpp"
#include "tiffio.h"
#include <iostream>

using namespace std;

adapter::adapter(bim::STRING file, bim::STRING mode) {
	/*
		ADAPTER - I/O adapters for reading and writing to TIFF files.
	 
		adapter_ = adapter(TIFF* tif) will create an adapter linked to the open 
		instance of TIFF image file.
	 */
	
	// Open TIFF File:
	TIFFSource = file;
	Mode = mode;
	tif = TIFFOpen(TIFFSource, Mode);

	readMetadata();
}

adapter::~adapter() {
	/*
		Adapter destructor - Closes open TIFF file.
	*/

	if (tif != nullptr) {
		TIFFClose((TIFF*)tif);
	}
}

cv::Mat adapter::readPatch(bim::UINT16 level, bim::LOCATION origin, 
							   bim::SIZE patchSize)
{
	/*
		BLK = readBlock(LEVEL, ORIGIN, BLOCKSIZE) reads a block of data specified
		at image coordinate ORIGIN and spanning a BLOCKSIZE of pixels in each image
		dimension. BLK is a cv::Mat image matrix containing pixel intensity data.
	*/

	// Point the TIFF object to specified directory level:
	TIFFSetDirectory((TIFF*)tif, level);

	// Get total number of bytes to allocate for the tile:
	tsize_t tileBytes_ = TIFFTileSize((TIFF*)tif);

	//TODO: Check image type to create buffer of specific image datatype.
	// Create tile buffer:
	std::vector<uint8_t> tile_;
	tile_.resize(tileBytes_);
	void* buffer_ = &(tile_[0]);

	TIFFReadTile((TIFF*)tif, buffer_, (uint32) origin[0], (uint32) origin[1], 0, 0);

	//Create cv::Mat object
	cv::Mat patch = cv::Mat((int) this->MetaData[level].TileSize[0], 
							(int) this->MetaData[level].TileSize[1],
							CV_8U, buffer_);
	return patch;
}

void adapter::readMetadata()
{
	/*
		METADATA(TIFF* tif) - Reads image metadata information from TIFF tags.
	 */

	// Get numer of directories:
	bim::UINT16 numDir_ = TIFFNumberOfDirectories((TIFF*)tif);
	MetaData = new METADATA[numDir_];

	bim::UINT16 dind_ = 0;
	do {
		/*
			Read metadata from each TIFF directory.
		*/
		TIFFGetField((TIFF*)tif, TIFFTAG_IMAGEWIDTH,      &(MetaData[dind_].Width));
		TIFFGetField((TIFF*)tif, TIFFTAG_IMAGELENGTH,     &(MetaData[dind_].Height));
		TIFFGetField((TIFF*)tif, TIFFTAG_ROWSPERSTRIP,    &(MetaData[dind_].RowsPerStrip));
		TIFFGetField((TIFF*)tif, TIFFTAG_SAMPLEFORMAT,    &(MetaData[dind_].SampleFormat));
		TIFFGetField((TIFF*)tif, TIFFTAG_TILELENGTH,      &(MetaData[dind_].TileSize[0]));
		TIFFGetField((TIFF*)tif, TIFFTAG_TILEWIDTH,       &(MetaData[dind_].TileSize[1]));
		TIFFGetField((TIFF*)tif, TIFFTAG_BITSPERSAMPLE,   &(MetaData[dind_].BitsPerSample));
		TIFFGetField((TIFF*)tif, TIFFTAG_SAMPLESPERPIXEL, &(MetaData[dind_].Channels));
		TIFFGetField((TIFF*)tif, TIFFTAG_COLORMAP,        &(MetaData[dind_].Colormap));
		TIFFGetField((TIFF*)tif, TIFFTAG_PLANARCONFIG,    &(MetaData[dind_].PlanarConfiguration));
		TIFFGetField((TIFF*)tif, TIFFTAG_PHOTOMETRIC,     &(MetaData[dind_].Photometric));

		// Increment directory counter
		dind_++;

	} while (TIFFReadDirectory((TIFF*)tif));

}