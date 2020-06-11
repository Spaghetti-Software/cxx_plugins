#include <gtest/gtest.h>

#include <tuple>

struct A;



TEST(CXXPlugins, HelloGTest) { EXPECT_EQ(true, true) << "Hello failed test!"; }






int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto result = RUN_ALL_TESTS();
  return result;
}