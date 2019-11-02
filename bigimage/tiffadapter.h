#pragma once

#include <cstdint>
#include "tiffio.h"
#include "bimtypes.h"

#ifndef BIGIMAGE_TIFFADAPTER
#define BIGIMAGE_TIFFADAPTER

using namespace bim;

class BIGIMAGE_DLL tiffadapter {

	/*
	 * Adapter Properties
	 */
public:
	// Adapter Properties:
	STRING TIFFSource;
	STRING Mode;
	const METADATA* MetaData;

private:
	TIFF* tif;

	/*
	 * Adapter methods
	 */
public:
	tiffadapter(STRING tiffile, STRING mode);
	~tiffadapter();
	void disp();

private:
	void getMetadata();
};

#endif //BIGIMAGE_TIFFADAPTER
