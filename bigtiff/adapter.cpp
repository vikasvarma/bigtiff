#include "adapter.hpp"
#include "tiffio.h"
#include <map>
#include <iostream>

using namespace std;
using namespace bim;

/*
	Define a cache map that stores neighboring patches.

	// TODO
*/

namespace {
	TIFF* tif;
	map<INDEX, cv::Mat> cache;
}


/*
------------------------------------------------------------------------------------
	API METHODS
------------------------------------------------------------------------------------
*/

adapter::adapter()
{
	// Default constructor. Don't do anything.
}

adapter::adapter(STRING file, STRING mode) {
	/*
		ADAPTER - I/O adapters for reading and writing to TIFF files.
	 
		adapter_ = adapter(TIFF* tif) will create an adapter linked to the open 
		instance of TIFF image file.
	 */
	
	// Open TIFF File:
	TIFFSource = file;
	Mode = mode;
	tif = TIFFOpen(TIFFSource, Mode);

	// Find the number of directories in BigTIFF:
	NumDirectories_ = TIFFNumberOfDirectories(tif);

	// Read meta data from each TIFF directory:
	readMetadata();
}

adapter::~adapter() {
	/*
		Adapter destructor - Closes open TIFF file.
	
	if (tif != nullptr) {
		TIFFClose(tif);
	}

	*/
}

UINT16 adapter::getNumDirectories() const { return NumDirectories_; }

bool adapter::istiled() const { return TIFFIsTiled(tif); }

bool adapter::isstriped() const { return !this->istiled(); }

cv::Mat adapter::readPatch(UINT16 level, INDEX pindex, SIZE patchSize)
{
	/*
		PATCH = readPatch(LEVEL, ORIGIN, PATCHSIZE) reads a patch of data specified
		at image coordinate ORIGIN and spanning a PATCHSIZE of pixels in each image
		dimension. PATCH is a cv::Mat image matrix containing pixel intensity data.
	*/

	// Find all I/O tiles required to stitch the patch:
	INDEX tileOrigin_;
	cv::Mat patch_;

	if (this->istiled())
	{
		//Tiled image:
		cv::Mat tileset_;
		
		/*
			Find top-left and bottom right of patch:
		*/
		vector<INDEX> patchCorners_ = extents(level, patchSize, pindex);

		// Map patch corners to tile locations:
		INDEX tltile_, brtile_;
		tltile_[0] = floor(patchCorners_[0][0] / MetaData[level].TileSize[0]);
		tltile_[1] = floor(patchCorners_[0][1] / MetaData[level].TileSize[1]);
		brtile_[0] = floor(patchCorners_[1][0] / MetaData[level].TileSize[0]);
		brtile_[1] = floor(patchCorners_[1][1] / MetaData[level].TileSize[1]);

		// Read all I/O tiles required:
		vector<cv::Mat> cols_;

		for (int m_ = tltile_[0]; m_ <= brtile_[0]; m_++)
		{
			cv::Mat rows_;
			vector<cv::Mat> tiles_;

			for (int n_ = tltile_[1]; n_ <= brtile_[0]; n_++)
			{
				tileOrigin_ = { static_cast<int64_t>(m_*MetaData[level].TileSize[0]),
								static_cast<int64_t>(n_*MetaData[level].TileSize[1]) };

				// Get tile:
				tiles_.push_back(readIOTile(level, tileOrigin_));
			}
			
			// Concatenate a column of tiles:
			vconcat(tiles_, rows_);
			cols_.push_back(rows_);
		}

		// Concatenate all columns:
		hconcat(cols_, tileset_);

		// Now, sub-index into the accumulated matrix:
		INDEX origin_ = { pindex[0] - tltile_[0] * MetaData[level].TileSize[0],
						  pindex[1] - tltile_[1] * MetaData[level].TileSize[1] };

		cv::Rect roi_ = cv::Rect(origin_[0], origin_[1], patchSize[0], patchSize[1]);
		patch_ = tileset_(roi_).clone();

	}
	else {
		// TODO: Striped images
	}

	return patch_;
}

cv::Mat adapter::readRegion(UINT16 level, LOCATION regStart, LOCATION regEnd) 
{
	/*
		TODO
	*/
	return cv::Mat();
}

/*
------------------------------------------------------------------------------------
	PRIVATE HELPER METHODS
------------------------------------------------------------------------------------
*/

//----------------------------------------------------------------------------------
void adapter::readMetadata()
{
	/*
		METADATA(TIFF* tif) - Reads image metadata information from TIFF tags.
	 */

	MetaData = new METADATA[NumDirectories_];

	UINT16 dind_ = 0;
	do {
		/*
			Read metadata from each TIFF directory.
		*/
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,      &(MetaData[dind_].Width));
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH,     &(MetaData[dind_].Height));
		TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP,    &(MetaData[dind_].RowsPerStrip));
		TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT,    &(MetaData[dind_].SampleFormat));
		TIFFGetField(tif, TIFFTAG_TILELENGTH,      &(MetaData[dind_].TileSize[0]));
		TIFFGetField(tif, TIFFTAG_TILEWIDTH,       &(MetaData[dind_].TileSize[1]));
		TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE,   &(MetaData[dind_].BitsPerSample));
		TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &(MetaData[dind_].Channels));
		TIFFGetField(tif, TIFFTAG_COLORMAP,        &(MetaData[dind_].Colormap));
		TIFFGetField(tif, TIFFTAG_PLANARCONFIG,    &(MetaData[dind_].PlanarConfiguration));
		TIFFGetField(tif, TIFFTAG_PHOTOMETRIC,     &(MetaData[dind_].Photometric));

		// Increment directory counter
		dind_++;

	} while (TIFFReadDirectory(tif));

}

//----------------------------------------------------------------------------------
vector<INDEX> adapter::extents(UINT16 level, SIZE unitsize, INDEX pindex)
{
	/*
		Determine the top-left and bottom-right edges of the unit based on a
		spatial location situated in it.
	*/

	vector<INDEX> extents_;

	// Find the unit to which this location belongs to:
	UINT64 m_ = floor(pindex[0] / unitsize[0]);
	UINT64 n_ = floor(pindex[1] / unitsize[1]);

	// Add top-left and bottom-right:
	INDEX tl_ = { m_*unitsize[0], n_*unitsize[1] };
	INDEX br_ = { (m_ + 1)*unitsize[0] - 1, (n_ + 1)*unitsize[1] - 1 };

	// Clamp indices to image extents:
	tl_[0] = std::max((UINT64)tl_[0], UINT64(0));
	tl_[1] = std::max((UINT64)tl_[1], UINT64(0));
	tl_[0] = std::min((UINT64)tl_[0], MetaData[level].Width);
	tl_[1] = std::min((UINT64)tl_[1], MetaData[level].Height);

	br_[0] = std::max((UINT64)br_[0], UINT64(0));
	br_[1] = std::max((UINT64)br_[1], UINT64(0));
	br_[0] = std::min((UINT64)br_[0], MetaData[level].Width);
	br_[1] = std::min((UINT64)br_[1], MetaData[level].Height);

	// Convert the image indices to spatial locations:
	extents_ = { tl_, br_ };

	return extents_;
}

//----------------------------------------------------------------------------------
cv::Mat adapter::readIOTile(UINT16 level, INDEX origin)
{
	/*
		Reads a TILE of data from file or from stored cache.
	*/

	// Point the TIFF object to specified directory level:
	TIFFSetDirectory(tif, level);

	// Get total number of bytes to allocate for the tile:
	UINT64 tileBytes_ = TIFFTileSize(tif);

	// Create tile buffer:
	void* buffer_ = _TIFFmalloc(tileBytes_);

	TIFFReadTile(tif, buffer_, (UINT32)origin[0], (UINT32)origin[1], 0, 0);

	//Create cv::Mat object
	cv::Mat tile_ = cv::Mat((int)this->MetaData[level].TileSize[0],
							(int)this->MetaData[level].TileSize[1],
							getDatatype(level), buffer_);
	return tile_;
}

//----------------------------------------------------------------------------------
UINT8 adapter::getDatatype(UINT16 level)
{
	/*
		Determines OpenCV datatype of the image from Metadata.
	*/

	UINT8 type;

	// Determine the type from sample format:
	switch (this->MetaData[level].SampleFormat)
	{
		case SAMPLEFORMAT_UINT:
			switch (this->MetaData[level].BitsPerSample) {
			case  8: type = CV_8U;
			case 16: type = CV_16U;
			}

		case SAMPLEFORMAT_INT:
			switch (this->MetaData[level].BitsPerSample) {
			case  8: type = CV_8S;
			case 16: type = CV_16S;
			case 32: type = CV_32S;
			}

		case SAMPLEFORMAT_IEEEFP:
			switch (this->MetaData[level].BitsPerSample) {
			case 16: type = CV_16F;
			case 32: type = CV_32F;
			case 64: type = CV_64F;
			}
	}

	// Further tune the datatype based on number of channels:
	switch (type) {
		case CV_8U:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_8UC1;
			case  2: type = CV_8UC2;
			case  3: type = CV_8UC3;
			default: type = CV_8UC(this->MetaData[level].Channels);
			}
				
		case CV_16U:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_16UC1;
			case  2: type = CV_16UC2;
			case  3: type = CV_16UC3;
			default: type = CV_16UC(this->MetaData[level].Channels);
			}

		case CV_8S:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_8SC1;
			case  2: type = CV_8SC2;
			case  3: type = CV_8SC3;
			default: type = CV_8SC(this->MetaData[level].Channels);
			}

		case CV_16S:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_16SC1;
			case  2: type = CV_16SC2;
			case  3: type = CV_16SC3;
			default: type = CV_16SC(this->MetaData[level].Channels);
			}

		case CV_32S:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_32SC1;
			case  2: type = CV_32SC2;
			case  3: type = CV_32SC3;
			default: type = CV_32SC(this->MetaData[level].Channels);
			}

		case CV_16F:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_16FC1;
			case  2: type = CV_16FC2;
			case  3: type = CV_16FC3;
			default: type = CV_16FC(this->MetaData[level].Channels);
			}

		case CV_32F:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_32FC1;
			case  2: type = CV_32FC2;
			case  3: type = CV_32FC3;
			default: type = CV_32FC(this->MetaData[level].Channels);
			}

		case CV_64F:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_64FC1;
			case  2: type = CV_64FC2;
			case  3: type = CV_64FC3;
			default: type = CV_64FC(this->MetaData[level].Channels);
			}
	} // Done

	return type;
}
