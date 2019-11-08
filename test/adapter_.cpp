#include "gtest/gtest.h"
#include "adapter.hpp"
#include "bimtypes.hpp"

using namespace bim;

TEST(TIFFADAPTER, Constructor) {
	STRING file = "tumor.tif";
	STRING mode = "r";

	adapter* adptr_ = &(adapter(file, mode));

	EXPECT_EQ(adptr_->MetaData[0].Width, bim::DOUBLE(5358));
	EXPECT_EQ(adptr_->MetaData[0].Height, bim::DOUBLE(5000));
	EXPECT_EQ(adptr_->MetaData[0].BitsPerSample, bim::DOUBLE(8));
}

TEST(TIFFADAPTER, readBlock) {
	STRING file = "tumor.tif";
	STRING mode = "r";

	adapter* adptr_ = &(adapter(file, mode));

	bim::LOCATION origin = { 1, 1 };
	bim::SIZE blockSize = { 256, 256 };

	cv::Mat patch = adptr_->readPatch(bim::UINT16(0), origin, blockSize);
}