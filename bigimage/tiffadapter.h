#pragma once

#include <cstdint>
#include "bimtypes.h"

#ifndef BIGIMAGE_TIFFADAPTER
#define BIGIMAGE_TIFFADAPTER

class BIGIMAGE_DLL tiffadapter {

	/*
	 * Adapter Properties
	 */
public:
	// Adapter Properties:
	bim::STRING TIFFSource;
	bim::STRING Mode;
	METADATA* MetaData;

private:
	void* tif;

	/*
	 * Adapter methods
	 */
public:
	tiffadapter(bim::STRING tiffile, bim::STRING mode);
	~tiffadapter();
	void disp();

private:
	void readMetadata();
};

#endif //BIGIMAGE_TIFFADAPTER
