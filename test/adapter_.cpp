#include <tuple>
#include "gtest/gtest.h"
#include "adapter.hpp"
#include "bimtypes.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace btiff;

/*
	For measure execution time:
*/
#include <chrono>
#include <iostream>
using namespace std::chrono;

namespace adapter_{
	vector<uchar> Mat2Vector(Mat image)
	{
		cv::Mat flat = image.reshape(1, image.total()*image.channels());
		std::vector<uchar> vec = image.isContinuous() ? flat : flat.clone();
		return vec;
	}

	bool equal(cv::Mat A_, cv::Mat B_)
	{
		vector<uchar> act = Mat2Vector(A_);
		vector<uchar> exp = Mat2Vector(B_);
		return (act == exp);
	}
}

class ReadFixture
	  : public ::testing::TestWithParam<tuple<UINT16, INDEX, SIZE, STRING>> {

protected:
	UINT16 Directory = get<0>(GetParam());
	INDEX  Origin    = get<1>(GetParam());
	SIZE   PatchSize = get<2>(GetParam());
	STRING Baseline  = get<3>(GetParam());

public:
	Mat Patch;
	bool Matched;

	virtual void SetUp()
	{
		STRING file = "tumor.tif";
		STRING mode = "r";

		// Create adapter:
		adapter* adptr_ = &(adapter(file, mode));

		// Start measuring
		auto start = high_resolution_clock::now();

		// Read patch
		Patch = adptr_->readPatch(Directory, Origin, PatchSize);

		// Stop measuring
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);

		cout << "[      LOG ] Execution time: " 
			 << duration.count()/1E6 
			 << " (seconds)" 
			 << endl;

		// Check if patch matches baseline:
		Mat exp_ = imread(Baseline);
		Matched = adapter_::equal(Patch, exp_);

		// Write patch out:
		imwrite("Patch.png", Patch);
	}
};

TEST_P(ReadFixture, ReadLargePatch)
{
	EXPECT_TRUE(Matched);
}

INSTANTIATE_TEST_CASE_P(
	ADAPTER,
	ReadFixture,
	::testing::Values(
		make_tuple(	UINT16(2),
					INDEX({   0,   0 }), 
					SIZE( { 512, 512 }), 
					STRING("ExactPatch.png")
		          ),
		make_tuple(	UINT16(0),
					INDEX({ 1560, 2340 }),
					SIZE( { 1025,  600 }),
					STRING("LargePatch.png")
				  ),
		make_tuple( UINT16(1),
					INDEX({ 1339, 1249 }),
					SIZE( {  256,  300 }),
					STRING("SmallPatch.png")
				  )
	),
);

TEST(ADAPTER, Constructor) {
	STRING file = "tumor.tif";
	STRING mode = "r";

	adapter* adptr_ = &(adapter(file, mode));

	EXPECT_EQ(adptr_->MetaData[0].Width, UINT64(5358));
	EXPECT_EQ(adptr_->MetaData[0].Height, UINT64(5000));
	EXPECT_EQ(adptr_->MetaData[0].BitsPerSample, UINT8(8));
}