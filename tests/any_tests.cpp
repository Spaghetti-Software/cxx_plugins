#include <gtest/gtest.h>
#include <cxx_plugins/any.hpp>

struct mem_block {
  void *ptr;
  std::size_t size;
};

template<std::size_t S>
class SingleStackAllocator {
public:
  mem_block allocate(std::size_t size) {
    if (size > S)
      throw std::runtime_error("");
    return {stack, size};
  }

  void deallocate([[maybe_unused]] void *ptr) {}

private:
  char stack[S];
};

TEST(AnyTest, AnyTest) {
  using namespace CxxPlugins::utility;
  using allocator_t = SingleStackAllocator<8>;
  using any = any<allocator_t>;
  any a1 = 1;

  std::cout << any_cast<int>(a1) << std::endl;

  a1 = 3.14;

  std::cout << any_cast<double>(a1) << std::endl;

  a1 = 3.5f;

  std::cout << any_cast<float>(a1) << std::endl;
}