#pragma once

/*
 * This header file captures all bigimage type aliases. Insert regularly used
 * variable type definitions here.
*/
#include <cstdint>

/*
Define DLL export and import macros:
*/
#ifdef BIGTIFF_EXPORTS
#    define BIGTIFF_DLL __declspec(dllexport)
#else
#    define BIGTIFF_DLL __declspec(dllimport)
#endif

namespace bim {
	/*
	 * SIZE - A 1-by-2 array of integer vector depicting the size along each
	 *        dimension of the entity it represents.
	 */
	using SIZE = uint64_t[2];

	/*
	 * SIZE - A 1-by-2 array of integer vector depicting the size along each
	 *        dimension of the entity it represents.
	 */
	using LOCATION = uint64_t[2];

	/*
	 * DOUBLE - 64-bit double precision floating point numerical.
	 */
	using DOUBLE = uint64_t;

	/*
	 * STRING - A character pointer alias to represent the fully qualified name
	 *        of a file on disk.
	 */
	using STRING = const char*;

	/*
	 * UINT8 - Overload to unsigned 8-bit integer
	 */
	using UINT8 = uint8_t;

	/*
	 * UINT16 - Overload to unsigned 16-bit integer
	 */
	using UINT16 = uint16_t;
}