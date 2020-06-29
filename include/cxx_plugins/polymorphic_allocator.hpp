/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    polymorphic_allocator.hpp
 * \author  Andrey Ponomarev
 * \date    25 Jun 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "cxx_plugins/allocator_helpers.hpp"
#include "cxx_plugins/polymorphic_ref.hpp"

#include <boost/type_index/runtime_cast/pointer_cast.hpp>
#include <memory_resource>

namespace CxxPlugins {

struct allocate {};
struct deallocate {};
struct isEqual {};

using MemoryResourceRef = PrimitivePolymorphicRef<
    TaggedSignature<allocate, void *(std::size_t bytes, std::size_t alignment)>,
    TaggedSignature<deallocate,
                void(void *p, std::size_t bytes, std::size_t alignment)>,
    TaggedSignature<isEqual, bool(const SelfType &) const>>;

template <typename T>
constexpr auto polymorphicExtend(allocate /*unused*/, T &mem_resource,
                                 std::size_t bytes, std::size_t alignment)
    -> void * {
  return mem_resource.allocate(bytes, alignment);
}

template <typename T>
constexpr void polymorphicExtend(deallocate /*unused*/, T &mem_resource,
                                 void *p, std::size_t bytes,
                                 std::size_t alignment) {
  mem_resource.deallocate(p, bytes, alignment);
}


template <typename T,
          typename = std::enable_if_t<
              !std::is_same_v<std::pmr::memory_resource, std::decay_t<T>> &&
              !std::is_base_of_v<std::pmr::memory_resource, std::decay_t<T>>>>
constexpr auto polymorphicExtend(isEqual /*unused*/, T const &mem_resource,
                                 MemoryResourceRef const &rhs) -> bool {
  return mem_resource.is_equal(rhs);
}

constexpr auto polymorphicExtend(isEqual, std::pmr::memory_resource const &lhs,
                                 MemoryResourceRef const &rhs) {
  if (rhs.isA<std::pmr::memory_resource>()) {
    return lhs.is_equal(
        *static_cast<std::pmr::memory_resource const *>(rhs.data()));
  }
  return false;
}

inline auto operator==(MemoryResourceRef const &lhs, MemoryResourceRef const &rhs)
    -> bool {
  return lhs.call<CxxPlugins::isEqual>(rhs);
}

// By default evaluates to std::pmr::get_default_resource()
auto getDefaultMemoryResource() noexcept -> MemoryResourceRef;
auto setDefaultMemoryResource(MemoryResourceRef mem_resource) noexcept
    -> MemoryResourceRef;
auto setDefaultMemoryResource(
    std::pmr::memory_resource *std_mem_resource_p) noexcept
    -> MemoryResourceRef;
auto setDefaultMemoryResource(
    std::pmr::memory_resource &std_mem_resource) noexcept -> MemoryResourceRef;

template <typename Tp = std::byte>
/*!
 * \brief
 * \details
 *
 * \note
 * Uses naming convention of std, so that it doesn't conflict with
 * std containers.
 *
 * \details
 *
 */
class PolymorphicAllocator {
public:
  using value_type = Tp;

  // constructors
  PolymorphicAllocator() noexcept = default;
  PolymorphicAllocator(MemoryResourceRef r) noexcept : resource_m(r) {}
  PolymorphicAllocator(std::pmr::memory_resource* resource) noexcept :
    resource_m(resource)
  {}
  PolymorphicAllocator(std::pmr::memory_resource& resource) noexcept :
      resource_m(resource)
  {}

  PolymorphicAllocator(const PolymorphicAllocator &other) = default;

  template <class U>
  PolymorphicAllocator(const PolymorphicAllocator<U> &other) noexcept
      : resource_m(other.resource()) {}

  PolymorphicAllocator &operator=(const PolymorphicAllocator &rhs) = delete;

  // member functions
  [[nodiscard]] Tp *allocate(std::size_t n) {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(Tp)) {
      cxxPluginsBadArrayLength();
    }

    return static_cast<Tp *>(
        resource_m.call<CxxPlugins::allocate>(n * sizeof(Tp), alignof(Tp)));
  }
  void deallocate(Tp *p, size_t n) noexcept {
    resource_m.call<CxxPlugins::deallocate>(p, n * sizeof(Tp), alignof(Tp));
  }

  [[nodiscard]] auto allocate_bytes(size_t nbytes,
                                    size_t alignment = alignof(max_align_t))
      -> void * {
    return resource_m.call<CxxPlugins::allocate>(nbytes, alignment);
  }
  void deallocate_bytes(void *p, size_t nbytes,
                        size_t alignment = alignof(max_align_t)) {
    resource_m.call<CxxPlugins::deallocate>(p, nbytes, alignment);
  }
  template <typename U>[[nodiscard]] auto allocate_object(size_t n = 1) -> U * {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(U)) {
      cxxPluginsBadArrayLength();
    }
    return static_cast<U *>(allocate_bytes(n * sizeof(U), alignof(U)));
  }
  template <typename U> void deallocate_object(U *p, size_t n = 1) {
    deallocate_bytes(p, n * sizeof(U), alignof(U));
  }
  template <typename U, class... CtorArgs>
  [[nodiscard]] U *new_object(CtorArgs &&... ctor_args) {
    U *p = allocate_object<U>();
#ifdef __cpp_exceptions
    try {
      construct(p, std::forward<CtorArgs>(ctor_args)...);
    } catch (...) {
      deallocate_object(p);
      throw;
    }
#else
    construct(p, std::forward<CtorArgs>(ctor_args)...);
#endif
    return p;
  }
  template <class T> void delete_object(T *p) {
    destroy(p);
    deallocate_object(p);
  }

  template <typename U, typename... Args>
  void construct(U *p, Args &&... args) {
    CxxPlugins::uninitializedConstructUsingAllocator(
        p, *this, std::forward<Args>(args)...);
  }

  template <typename U> void destroy(U *p) { p->~U(); }

  auto select_on_container_copy_construction() const -> PolymorphicAllocator {
    return PolymorphicAllocator();
  }

  MemoryResourceRef resource() const { return resource_m; }

private:
  MemoryResourceRef resource_m = getDefaultMemoryResource();
};

template <typename T1, typename T2>
constexpr auto
operator==(const CxxPlugins::PolymorphicAllocator<T1> &lhs,
           const CxxPlugins::PolymorphicAllocator<T2> &rhs) noexcept -> bool {
  return lhs.resource() == rhs.resource();
}

} // namespace CxxPlugins