#include "gtest/gtest.h"
#include "spatialmap.hpp"

using namespace bim;

TEST(SPATIALMAP, Constructor)
{
	RANGE xlim   = { 0 - 0.5, 40 - 0.5 };
	RANGE ylim   = { 0 - 0.5, 20 - 0.5 };
	SIZE  imsize = { 40, 20 };
	
	spatialmap map_ = spatialmap(xlim, ylim, imsize);

	EXPECT_EQ(map_.XLimits, xlim);
	EXPECT_EQ(map_.YLimits, ylim);
	EXPECT_EQ(map_.ImageSize, imsize);
	EXPECT_EQ(map_.XSpacing, 1);
	EXPECT_EQ(map_.YSpacing, 1);
}

TEST(SPATIALMAP, spatialToImage)
{
	RANGE xlim   = { 0 - 0.5, 25 - 0.5 };
	RANGE ylim   = { 0 - 0.5, 44 - 0.5 };
	SIZE  imsize = { 10, 20 };

	spatialmap map_ = spatialmap(xlim, ylim, imsize);

	// Verify input properties:
	EXPECT_EQ(map_.XLimits, xlim);
	EXPECT_EQ(map_.YLimits, ylim);
	EXPECT_EQ(map_.ImageSize, imsize);

	// Verify derived properties:
	EXPECT_EQ(map_.XSpacing, 2.5);
	EXPECT_EQ(map_.YSpacing, 2.2);

	// Location 1: [1.2 4.5]
	LOCATION xy_  = { 1.2, 4.5 };
	INDEX pixind_ = map_.spatialToImage(xy_);
	INDEX exp_ = { 0, 2 };

	EXPECT_EQ(pixind_, exp_);
	
	// Location 2: []
}