#include "tiffadapter.h"
#include "tiffio.h"
#include <iostream>
using namespace std;

tiffadapter::tiffadapter(const STRING tiffile, const STRING mode) {
	/*
	 * TIFFADAPTER - TIFF Adapter is an I/O adapters for reading and writing
	 *               to TIFF files.
	 *
	 * adapters = tiffadapter(TIFFFILE, MODE) will create an adapters linked to
	 * a TIFF image file (TIFFFILE) on disk. The MODE specifies whether the
	 * adapters is configured to read/write to the image.
	 */

	TIFFSource = tiffile;
	Mode = mode;
	tif = TIFFOpen(tiffile, mode);
	getMetadata();
}

tiffadapter::~tiffadapter() {
	/*
	 * TIFFADAPTER class destructor. Closes open TIFF object.
	 */
	TIFFClose(tif);
	delete this;
}

void tiffadapter::getMetadata() {
	/*
	 * getMetadata - Reads metadata information from TIFF tags.
	 */

	 // Set metadata fields:
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &MetaData->Width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &MetaData->Height);
	TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &MetaData->RowsPerStrip);
	TIFFGetField(tif, TIFFTAG_DATATYPE, &MetaData->Datatype);
	TIFFGetField(tif, TIFFTAG_TILELENGTH, &MetaData->TileSize[0]);
	TIFFGetField(tif, TIFFTAG_TILEWIDTH, &MetaData->TileSize[1]);
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &MetaData->BitsPerSample);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &MetaData->Channels);
	TIFFGetField(tif, TIFFTAG_COLORMAP, &MetaData->Colormap);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &MetaData->PlanarConfiguration);
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &MetaData->PhotometricInterpretation);
}

void tiffadapter::disp() {
	cout << "\nTIFFAdapter with properties:";
	cout << "\nSource:" << TIFFSource;
	cout << "\nMode:" << Mode;
	cout << "\nMetadata";
	cout << "\n\tImageSize:[" << MetaData->Height << "," << MetaData->Width << "]";
	cout << "\n\tChannels:" << MetaData->Channels;
}