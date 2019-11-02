#pragma once

/*
 * This header file captures all bigimage type aliases. Insert regularly used
 * variable type definitions here.
*/
#include <cstdint>

/*
Define DLL export and import macros:
*/
#ifdef BIGIMAGE_EXPORTS
#    define BIGIMAGE_DLL __declspec(dllexport)
#else
#    define BIGIMAGE_DLL __declspec(dllimport)
#endif

#ifndef BIGIMAGE_BIMTYPES
#define BIGIMAGE_BIMTYPES

namespace bim {
	/*
	 * SIZE - A 1-by-2 array of integer vector depicting the size along each
	 *        dimension of the entity it represents.
	 */
	using SIZE = uint64_t* ;

	/*
	 * DOUBLE - 64-bit double precision floating point numerical.
	 */
	using DOUBLE = uint64_t* ;

	/*
	 * STRING - A character pointer alias to represent the fully qualified name
	 *        of a file on disk.
	 */
	using STRING = char*;
}

struct METADATA {
	/*
	 * METADATA - Structure that stores the metadata information of a TIFF
	 *            bigimage. This metadata information is read from TIFF tags.
	 */

	const bim::SIZE TileSize;
	const bim::DOUBLE RowsPerStrip;
	const bim::DOUBLE Height;
	const bim::DOUBLE Width;
	const bim::DOUBLE BitsPerSample;
	const bim::DOUBLE Channels;
	const bim::STRING PlanarConfiguration;
	const bim::STRING PhotometricInterpretation;
	const bim::STRING Format;
	const bim::STRING Datatype;
	const bim::DOUBLE Colormap;
};

#endif //BIGIMAGE_BIMTYPES