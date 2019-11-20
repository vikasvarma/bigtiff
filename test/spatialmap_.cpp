#include "gtest/gtest.h"
#include "spatialmap.hpp"

using namespace btiff;

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
	// NOTE: Declate test variables that change values multiple times.
	INDEX pind_, exp_;
	LOCATION xy_;

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

	/*
		Location 1: [1.2 4.5]
		... A location in the [0 2] pixel, which is not the centre of the pixel.
	*/
	xy_   = { 1.2, 4.5 };
	pind_ = map_.spatialToImage(xy_);
	exp_  = { 0, 2 };
	EXPECT_EQ(pind_, exp_);
	
	/*
		Location 2: [0.5 0.5]
		... Location in the first image pixel, not centre.
	*/
	xy_ = { 0.5, 0.5 };
	pind_ = map_.spatialToImage(xy_);
	exp_ = { 0, 0 };
	EXPECT_EQ(pind_, exp_);

	/*
		Location 2: [24.5 43.5]
		... Location in the first image pixel, not centre.
	*/
	xy_ = { 24.5, 43.5 };
	pind_ = map_.spatialToImage(xy_);
	exp_ = { 10, 20 };
	EXPECT_EQ(pind_, exp_);

}

TEST(SPATIALMAP, imageToSpatial)
{
	// NOTE: Declate test variables that change values multiple times.
	INDEX pind_;
	LOCATION xy_, exp_;

	RANGE xlim = { 0 - 0.5, 25 - 0.5 };
	RANGE ylim = { 0 - 0.5, 44 - 0.5 };
	SIZE  imsize = { 10, 20 };

	spatialmap map_ = spatialmap(xlim, ylim, imsize);

	// Verify input properties:
	EXPECT_EQ(map_.XLimits, xlim);
	EXPECT_EQ(map_.YLimits, ylim);
	EXPECT_EQ(map_.ImageSize, imsize);

	// Verify derived properties:
	EXPECT_EQ(map_.XSpacing, 2.5);
	EXPECT_EQ(map_.YSpacing, 2.2);

	/*
		Index 1: [5 6]
		... An index inside the image, when converted to spatial coordinates, the 
		spatial coordinate for the centre of the pixel is returned.
	*/
	pind_ = { 5, 6};
	xy_   = map_.imageToSpatial(pind_);
	exp_  = { 13.25, 13.8 };
	EXPECT_EQ(xy_, exp_);

	/*
		Index 1: [0 0]
		... Indexing from the first pixel coordinate, will map to the centre of the 
		pixel and not the world origin.
	*/
	pind_ = { 0, 0 };
	xy_ = map_.imageToSpatial(pind_);
	exp_ = { 0.75, 0.6 };

	// NOTE: precision is not perfect. This should not be an issue (hopefully).
	DOUBLE prc_ = 1000000;
	EXPECT_EQ(round(xy_[0] * prc_) / prc_, round(exp_[0] * prc_) / prc_);
	EXPECT_EQ(round(xy_[1] * prc_) / prc_, round(exp_[1] * prc_) / prc_);

}