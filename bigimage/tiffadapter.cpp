#include "tiffadapter.h"
#include "tiffio.h"
#include <iostream>
using namespace std;

tiffadapter::tiffadapter(bim::STRING tiffile, bim::STRING mode) {
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
	readMetadata();
}

tiffadapter::~tiffadapter() {
	/*
	 * TIFFADAPTER class destructor. Closes open TIFF object.
	 */
	if (tif != nullptr) {
		TIFFClose((TIFF*)tif);
	}
}

void tiffadapter::readMetadata() {
	/*
	 * getMetadata - Reads metadata information from TIFF tags.
	 */

	//if (MetaData == nullptr) {
		METADATA meta = {
			{0,0}, //TileSize
		        0, //RowsPerStrip
				0, //Height
				0, //Width
				0, //BitsPerSample
				0, //Channels
		  nullptr, //PlanarConfiguration
		  nullptr, //PhotometricInterpretation
		  nullptr, //Format
		  nullptr, //Datatype
				0, //Colormap
		};

		MetaData = &(meta);
	//}
	
	 // Set metadata fields:
	TIFFGetField((TIFF*)tif, TIFFTAG_IMAGEWIDTH, &(MetaData->Width));
	TIFFGetField((TIFF*)tif, TIFFTAG_IMAGELENGTH, &(MetaData->Height));
	TIFFGetField((TIFF*)tif, TIFFTAG_ROWSPERSTRIP, &(MetaData->RowsPerStrip));
	//TIFFGetField((TIFF*)tif, TIFFTAG_DATATYPE, &MetaData->Datatype);
	TIFFGetField((TIFF*)tif, TIFFTAG_TILELENGTH, &(MetaData->TileSize[0]));
	TIFFGetField((TIFF*)tif, TIFFTAG_TILEWIDTH, &(MetaData->TileSize[1]));
	TIFFGetField((TIFF*)tif, TIFFTAG_BITSPERSAMPLE, &(MetaData->BitsPerSample));
	TIFFGetField((TIFF*)tif, TIFFTAG_SAMPLESPERPIXEL, &(MetaData->Channels));
	TIFFGetField((TIFF*)tif, TIFFTAG_COLORMAP, &(MetaData->Colormap));
	//TIFFGetField((TIFF*)tif, TIFFTAG_PLANARCONFIG, &MetaData->PlanarConfiguration);
	//TIFFGetField((TIFF*)tif, TIFFTAG_PHOTOMETRIC, &MetaData->PhotometricInterpretation);
}

void tiffadapter::disp() {
	cout << "\nTIFFAdapter with properties:";
	cout << "\nSource:" << TIFFSource;
	cout << "\nMode:" << Mode;
	cout << "\nMetadata";
	cout << "\n\tImageSize:[" << MetaData->Height << "," << MetaData->Width << "]";
	cout << "\n\tChannels:" << MetaData->Channels;
}