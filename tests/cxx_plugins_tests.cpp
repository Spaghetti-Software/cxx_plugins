#include <gtest/gtest.h>

struct A;

TEST(CXXPlugins, HelloGTest) { EXPECT_EQ(true, true) << "Hello failed test!"; }

void foo() { auto ptr = new int(); }

int main(int argc, char **argv) {
  foo();
  ::testing::InitGoogleTest(&argc, argv);
  auto result = RUN_ALL_TESTS();
  return result;
}