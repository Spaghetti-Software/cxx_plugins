#include <gtest/gtest.h>

TEST(CXXPlugins, HelloGTest) 
{
  EXPECT_EQ(true, true) << "Hello failed test!";
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}