#include "gtest/gtest.h"
#include "adapter.hpp"
#include "bimtypes.hpp"

using namespace bim;

TEST(ADAPTER, Constructor) {
	STRING file = "tumor.tif";
	STRING mode = "r";

	adapter* adptr_ = &(adapter(file, mode));

	EXPECT_EQ(adptr_->MetaData[0].Width, UINT64(5358));
	EXPECT_EQ(adptr_->MetaData[0].Height, UINT64(5000));
	EXPECT_EQ(adptr_->MetaData[0].BitsPerSample, UINT8(8));
}

TEST(ADAPTER, readPatch) {
	STRING file = "tumor.tif";
	STRING mode = "r";

	adapter* adptr_ = &(adapter(file, mode));

	LOCATION origin = { 1, 1 };
	SIZE blockSize = { 256, 256 };
}