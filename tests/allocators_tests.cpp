//
// Created by Timur on 5/12/20.
//
#include <cxx_plugins/memory/stack_allocator.hpp>
#include <cxx_plugins/memory/mallocator.hpp>
#include <gtest/gtest.h>
#include <cstddef>

struct AllocatorsTests : public ::testing::Test {
  utility::mem_block block;
  void *allocator = nullptr;

  template <class Allocator>
  Allocator* createAllocator() {
    auto* p = new Allocator();
    allocator = p;
    return p;
  }

  template <class Allocator> void deleteAllocator() {
    delete reinterpret_cast<Allocator*>(allocator);
  }

  template<class Allocator>
  void allocate(std::size_t allocationSize, std::size_t alignment = 4) {
    auto* a = reinterpret_cast<Allocator*>(allocator);

    block = a->allocate(allocationSize, alignment);
  }

  template <class Allocator>
  void deallocate(utility::mem_block block) {
    auto *a = reinterpret_cast<Allocator *>(allocator);

    a->deallocate(block);
  }

  template <class Allocator>
  void testAllocation(utility::mem_block block, std::size_t expectedSize) const {
    EXPECT_NE(nullptr, block.ptr);
    EXPECT_EQ(expectedSize, block.size);

    const auto* a = reinterpret_cast<const Allocator*>(allocator);
    EXPECT_EQ(true, a->owns(block));
  }

  void testAllignment(utility::mem_block block, std::size_t expectedAllignment) const {
    const auto pVal = reinterpret_cast<std::uint64_t>(block.ptr);
    EXPECT_EQ(0, pVal % expectedAllignment);
  }

  utility::mem_block getLastAllocationBlock() const {
    return block;
  }
};

TEST_F(AllocatorsTests, StackAllocatorTest) {

  // single allocation of 4 bytes aligned by 4

  using allocatorT1 = utility::StackAllocator<4>;
  createAllocator<allocatorT1>();

  allocate<allocatorT1>(4, 4);
  testAllocation<allocatorT1>(getLastAllocationBlock(), 4);
  testAllignment(getLastAllocationBlock(), 4);

  deleteAllocator<allocatorT1>();

  // two allocations with different alignment
  // first allocation of 4 bytes aligned by 4
  // second of 4 bytes aligned by 6.
  // this should fill up the allocator with
  // 14 bytes of available storage
  // (assuming the initial alignment of the stack was 4
  // and no padding was needed for the first allocation)

  using allocatorT2 = utility::StackAllocator<14>;
  createAllocator<allocatorT2>();

  allocate<allocatorT2>(4, 4);
  testAllocation<allocatorT2>(getLastAllocationBlock(), 4);
  testAllignment(getLastAllocationBlock(), 4);
  deallocate<allocatorT1>(getLastAllocationBlock());

  allocate<allocatorT2>(4, 6);
  testAllocation<allocatorT2>(getLastAllocationBlock(), 6);
  testAllignment(getLastAllocationBlock(), 6);
  deallocate<allocatorT1>(getLastAllocationBlock());

  deleteAllocator<allocatorT2>();
}

TEST_F(AllocatorsTests, MallocatorTest) {
  using allocatorT1 = utility::Mallocator;
  createAllocator<allocatorT1>();

  allocate<allocatorT1>(4, 4);
  testAllocation<allocatorT1>(getLastAllocationBlock(), 4);
  testAllignment(getLastAllocationBlock(), 4);
  deallocate<allocatorT1>(getLastAllocationBlock());

  allocate<allocatorT1>(5, 16);
  testAllocation<allocatorT1>(getLastAllocationBlock(), 16);
  testAllignment(getLastAllocationBlock(), 16);
  deallocate<allocatorT1>(getLastAllocationBlock());

  deleteAllocator<allocatorT1>();
}