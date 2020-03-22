#include "adapter.hpp"
#include "tiffio.h"
#include <map>
#include <iostream>

using namespace std;
using namespace btiff;
using namespace cv;

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

//----------------------------------------------------------------------------------
adapter::adapter()
{
	// Default constructor. Don't do anything. Added to overcome build issues.
}

//----------------------------------------------------------------------------------
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
	NumDirectories = TIFFNumberOfDirectories(tif);

	// Read meta data from each TIFF directory:
	readMetadata();
}

//----------------------------------------------------------------------------------
adapter::~adapter() {
	/*
		Adapter destructor - Closes open TIFF file.
	
	if (tif != nullptr) {
		TIFFClose(tif);
	}

	*/
}

//----------------------------------------------------------------------------------
UINT16 adapter::getNumDirectories() const { return NumDirectories; }

//----------------------------------------------------------------------------------
bool adapter::istiled() const { return TIFFIsTiled(tif); }

//----------------------------------------------------------------------------------
bool adapter::isstriped() const { return !this->istiled(); }

//----------------------------------------------------------------------------------
cv::Mat adapter::readPatch(UINT16 level, INDEX pindex, SIZE patchSize)
{
	/*
		PATCH = readPatch(LEVEL, ORIGIN, PATCHSIZE) reads a patch of data specified
		at image coordinate ORIGIN and spanning a PATCHSIZE of pixels in each image
		dimension. PATCH is a cv::Mat image matrix containing pixel intensity data.
	*/

	/*
		INPUT VALIDATION:
		
		TODO
	*/
	stringstream err;

	// Check level:
	if (level < 0 || level >= NumDirectories)
	{
		throw "Invalid directory level.";
	}

	// Check to ensure that image coordinate supplied is within image extents:
	if (pindex[0] < 0 || pindex[0] > MetaData[level].Width ||
		pindex[1] < 0 || pindex[1] > MetaData[level].Height)
	{
		err << "Image Coordinate " << "[" << pindex[0] << ", " << pindex[1] << "]"
			<< " is outside image extents " 
			<< "[" << MetaData[level].Width << ", " << MetaData[level].Height << "].";
		throw err;
	}

	// Check to ensure that patchSize is valid:
	if (patchSize[0] < 0 || patchSize[1] < 0)
	{
		throw "Invalid PatchSize.";
	}

	// Initialize the output matrix:
	Mat patch_ = Mat::zeros(Size(patchSize[0], patchSize[1]),
							getDatatype(level));

	// Local variable that stores tile origins for all read tiles.
	INDEX tileOrigin_;

	if (this->istiled())
	{
		//Tiled image:
		Mat tileset_;
		
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
		vector<Mat> cols_;

		for (int m_ = tltile_[0]; m_ <= brtile_[0]; m_++)
		{
			Mat rows_;
			vector<Mat> tiles_;

			for (int n_ = tltile_[1]; n_ <= brtile_[1]; n_++)
			{
				tileOrigin_ = { static_cast<int64_t>(m_*MetaData[level].TileSize[0]),
								static_cast<int64_t>(n_*MetaData[level].TileSize[1]) };

				// Get tile:
				tiles_.push_back(readIOTile(level, tileOrigin_));
			}
			
			// Concatenate a column of tiles:
			if (tiles_.size() > 1) {
				vconcat(tiles_, rows_);
			}
			else { // Single tile only required.
				rows_ = tiles_[0];
			}

			cols_.push_back(rows_);
		}

		// Concatenate all columns:
		if (cols_.size() > 1) {
			hconcat(cols_, tileset_);
		}
		else { // Single tile only required.
			tileset_ = cols_[0];
		}

		// Now, sub-index into the accumulated matrix:
		INDEX origin_ = { patchCorners_[0][0] - (tltile_[0] * MetaData[level].TileSize[0]),
						  patchCorners_[0][1] - (tltile_[1] * MetaData[level].TileSize[1]) };

		Rect roi_ = Rect(origin_[0], origin_[1], patchSize[0], patchSize[1]);
		Mat  cropped_ = tileset_(roi_);

		cropped_.copyTo(patch_);
	}
	else {
		// TODO: Striped images
	}

	return patch_;
}

//----------------------------------------------------------------------------------
cv::Mat adapter::readRegion(UINT16 level, LOCATION regStart, LOCATION regEnd) 
{
	/*
		TODO
	*/
	return cv::Mat();
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

	MetaData = new METADATA[NumDirectories];

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
cv::Mat adapter::readIOTile(UINT16 level, INDEX origin)
{
	/*
		Reads a TILE of data from file or from stored cache.
	*/

	// Declare necessary variables:
	UINT16 NumChannels_ = MetaData[level].Channels;
	void* buffer_;
	UINT64 tileBytes_;
	Mat tile_ = Mat();

	// Point the TIFF object to specified directory level:
	TIFFSetDirectory(tif, level);

	switch (MetaData[level].PlanarConfiguration)
	{
	case PLANARCONFIG_CONTIG:
		// Get total number of bytes to allocate for the tile:
		tileBytes_ = TIFFTileSize(tif) * NumChannels_;

		// Create tile buffer:
		buffer_ = _TIFFmalloc(tileBytes_);

		TIFFReadTile(tif, buffer_, (UINT32)origin[0], (UINT32)origin[1], 0, 0);

		//Create cv::Mat object
		tile_ = Mat((int)this->MetaData[level].TileSize[0],
					(int)this->MetaData[level].TileSize[1],
					getDatatype(level), buffer_);

		// DONE
		break;

	case PLANARCONFIG_SEPARATE:
		// Get total number of bytes to allocate for the tile:
		tileBytes_ = TIFFTileSize(tif);
		vector<Mat> Channels_;
		Mat chtile_;

		/*
			NOTE: Always read from Nth channel to 0th channel. This is how OpenCV 
				  perceives multi-channel images.
				  For example:
					  RGB images are stored as B[0]G[1]R[2].
		*/
		for (int ch_ = MetaData[level].Channels - 1;
			 ch_ >= 0;
			 ch_--)
		{
			// Read specified channel sample:
			buffer_ = _TIFFmalloc(tileBytes_);
			TIFFReadTile(tif, buffer_, (UINT32)origin[0], (UINT32)origin[1], 0, 
						 static_cast<UINT16>(ch_));

			// Create cv::Mat and push to stack:
			chtile_ = Mat((int)this->MetaData[level].TileSize[0],
						  (int)this->MetaData[level].TileSize[1], 
						  CV_8UC1, buffer_); // TODO
			Channels_.push_back(chtile_);
		}

		// cv::Merge all channels:
		merge(Channels_, tile_);

		break;
	}

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
	switch ((UINT8) this->MetaData[level].SampleFormat)
	{
		case SAMPLEFORMAT_UINT:
			switch (this->MetaData[level].BitsPerSample) {
			case  8: type = CV_8U;  break;
			case 16: type = CV_16U; break;
			}
			break;

		case SAMPLEFORMAT_INT:
			switch (this->MetaData[level].BitsPerSample) {
			case  8: type = CV_8S;  break;
			case 16: type = CV_16S; break;
			case 32: type = CV_32S; break;
			}
			break;

		case SAMPLEFORMAT_IEEEFP:
			switch (this->MetaData[level].BitsPerSample) {
			case 16: type = CV_16F; break;
			case 32: type = CV_32F; break;
			case 64: type = CV_64F; break;
			}
			break;
	}

	// Further tune the datatype based on number of channels:
	switch (type) {
		case CV_8U:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_8UC1; break;
			case  2: type = CV_8UC2; break; 
			case  3: type = CV_8UC3; break; 
			default: type = CV_8UC(this->MetaData[level].Channels);
			}
			break;
				
		case CV_16U:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_16UC1; break;
			case  2: type = CV_16UC2; break;
			case  3: type = CV_16UC3; break;
			default: type = CV_16UC(this->MetaData[level].Channels); break;
			}
			break;

		case CV_8S:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_8SC1; break;
			case  2: type = CV_8SC2; break;
			case  3: type = CV_8SC3; break;
			default: type = CV_8SC(this->MetaData[level].Channels); break;
			}
			break;

		case CV_16S:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_16SC1; break;
			case  2: type = CV_16SC2; break;
			case  3: type = CV_16SC3; break;
			default: type = CV_16SC(this->MetaData[level].Channels); break;
			}
			break;

		case CV_32S:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_32SC1; break;
			case  2: type = CV_32SC2; break;
			case  3: type = CV_32SC3; break;
			default: type = CV_32SC(this->MetaData[level].Channels); break;
			}
			break;

		case CV_16F:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_16FC1; break;
			case  2: type = CV_16FC2; break;
			case  3: type = CV_16FC3; break;
			default: type = CV_16FC(this->MetaData[level].Channels); break;
			}
			break;

		case CV_32F:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_32FC1; break;
			case  2: type = CV_32FC2; break;
			case  3: type = CV_32FC3; break;
			default: type = CV_32FC(this->MetaData[level].Channels); break;
			}
			break;

		case CV_64F:
			switch (this->MetaData[level].Channels) {
			case  1: type = CV_64FC1; break;
			case  2: type = CV_64FC2; break;
			case  3: type = CV_64FC3; break;
			default: type = CV_64FC(this->MetaData[level].Channels); break;
			}
			break;
	} // Done

	return type;
}
