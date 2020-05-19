//
// Created by Timur on 5/12/20.
//
#include <cxx_plugins/memory/stack_allocator.hpp>
#include <cxx_plugins/memory/alligned_allocator.hpp>
#include <cxx_plugins/memory/mallocator.hpp>
#include <gtest/gtest.h>

struct AllocatorsTests : public ::testing::Test {
  template<class Allocator>
  void testAllocationSize(size_t allocationSize, size_t expectedSize) {
    Allocator a;

    mem_block block = a.allocate(allocationSize);

    EXPECT_EQ(expectedSize, block.size);
    EXPECT_EQ(true, a.owns(block));

    a.deallocate(block);
  }
};

TEST_F(AllocatorsTests, StackAllocatorTest) {
  testAllocationSize<utility::StackAllocator<4>>(4, 4);
}

TEST_F(AllocatorsTests, MallocatorTest) {
  testAllocationSize<utility::Mallocator>(4, 4);
}

TEST_F(AllocatorsTests, AllignedAllocatorTest) {
  testAllocationSize<utility::AllignedAllocator<utility::StackAllocator<32>, 32>>(4, 32);

  testAllocationSize<utility::AllignedAllocator<utility::StackAllocator<1000>, 32>>(159, 160);
}