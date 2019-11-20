#include "gtest/gtest.h"
#include "adapter.hpp"
#include "bimtypes.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace btiff;

TEST(ADAPTER, Constructor) {
	STRING file = "tumor.tif";
	STRING mode = "r";

	adapter* adptr_ = &(adapter(file, mode));

	EXPECT_EQ(adptr_->MetaData[0].Width, UINT64(5358));
	EXPECT_EQ(adptr_->MetaData[0].Height, UINT64(5000));
	EXPECT_EQ(adptr_->MetaData[0].BitsPerSample, UINT8(8));
}

TEST(ADAPTER, ExactPatch) {
	STRING file = "tumor.tif";
	STRING mode = "r";

	adapter* adptr_ = &(adapter(file, mode));

	// Read Paramters
	UINT16 directory_ = 2;
	INDEX     origin_ = { 0, 0 };
	SIZE   patchSize_ = { 256, 256 };

	Mat patch_ = adptr_->readPatch(directory_, origin_, patchSize_);
}

TEST(ADAPTER, LargePatch) {
	STRING file = "tumor.tif";
	STRING mode = "r";

	adapter* adptr_ = &(adapter(file, mode));

	LOCATION origin = { 1, 1 };
	SIZE blockSize = { 256, 256 };
}

TEST(ADAPTER, SmallPatch) {
	STRING file = "tumor.tif";
	STRING mode = "r";

	adapter* adptr_ = &(adapter(file, mode));

	LOCATION origin = { 1, 1 };
	SIZE blockSize = { 256, 256 };
}