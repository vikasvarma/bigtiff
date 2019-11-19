#pragma once

/*
 * This header file captures all bigimage type aliases. Insert regularly used
 * variable type definitions here.
*/
#include <cstdint>
#include <array>

/*
Define DLL export and import macros:
*/
#ifdef BIGTIFF_EXPORTS
#    define BIGTIFF_DLL __declspec(dllexport)
#else
#    define BIGTIFF_DLL __declspec(dllimport)
#endif

namespace bim {

	using SIZE     = std::array<uint64_t, 2>;
	using INDEX    = std::array<int64_t, 2>;
	using RANGE    = std::array<double, 2>;
	using LOCATION = std::array<double, 2>;
	using STRING   = const char*;
	using DOUBLE   = double;
	using UINT8	   = uint8_t;
	using UINT16   = uint16_t;
	using UINT32   = uint32_t;
	using UINT64   = uint64_t;

}