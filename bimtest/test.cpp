#include "gtest/gtest.h"
#include "tiffadapter.h"

TEST(TestCaseName, TestName) {
  

  char* file = "lena.tiff";
  char* mode = "r";

  tiffadapter tif = tiffadapter(file, mode);
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}