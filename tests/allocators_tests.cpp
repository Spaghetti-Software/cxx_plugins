//
// Created by Timur on 5/12/20.
//
#include <cxx_plugins/memory/stack_allocator.hpp>
#include <cxx_plugins/memory/alligned_allocator.hpp>
#include <cxx_plugins/memory/mallocator.hpp>
#include <gtest/gtest.h>
#include <cstddef>

struct AllocatorsTests : public ::testing::Test {
  mem_block block;

  template<class Allocator>
  void testAllocation(size_t allocationSize, size_t expectedSize) {
    Allocator a;

    block = a.allocate(allocationSize);

    EXPECT_NE(nullptr, block.ptr);
    EXPECT_EQ(expectedSize, block.size);
    EXPECT_EQ(true, a.owns(block));

    a.deallocate(block);
  }

  void testAllignment(size_t expectedAllignment) const {
    auto block = getBlock();
    uint64_t pVal = reinterpret_cast<uint64_t>(block.ptr);
    EXPECT_EQ(0, pVal % expectedAllignment);
  }

  mem_block getBlock() const {
    return block;
  }
};

TEST_F(AllocatorsTests, StackAllocatorTest) {
  testAllocation<utility::StackAllocator<4>>(4, 4);
  testAllignment(4);
}

TEST_F(AllocatorsTests, MallocatorTest) {
  testAllocation<utility::Mallocator>(4, 4);
  testAllignment(4);
}

TEST_F(AllocatorsTests, AllignedAllocatorTest) {
  testAllocation<utility::AllignedAllocator<utility::StackAllocator<32>, 32>>(
      4, 32);
  testAllignment(32);

  testAllocation<utility::AllignedAllocator<utility::StackAllocator<1000>, 32>>(
      159, 160);
  testAllignment(32);
}