#include "gtest/gtest.h"
#include "tiffadapter.h"
#include "bimtypes.h"

using namespace bim;

TEST(TestCaseName, TestName) {
	STRING file = "lena.tiff";
	STRING mode = "r";

	tiffadapter adapter = tiffadapter(file, mode);

	EXPECT_EQ(adapter.MetaData->Width, bim::DOUBLE(512));
}