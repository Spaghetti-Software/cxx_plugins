/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    polymorphic_allocator.cpp
 * \author  Andrey Ponomarev
 * \date    26 Jun 2020
 * \brief
 *
 */
#include "cxx_plugins/polymorphic_allocator.hpp"
#include "cxx_plugins/memory/mallocator.hpp"

#include <memory_resource>
#include <mutex>

namespace plugins {
namespace {
struct DefaultResource {
  std::mutex mutex;
  ::utility::Mallocator mallocator = {};
  MemoryResourcePtr resource = &mallocator;
};
} // namespace

static auto getDefaultResource() noexcept -> DefaultResource & {
  static DefaultResource resource = {};
  return resource;
}

auto getDefaultMemoryResource() noexcept -> MemoryResourcePtr {
  MemoryResourcePtr result;
  auto &resource = getDefaultResource();
  {
    std::scoped_lock lock(resource.mutex);
    result = resource.resource;
  }
  return result;
}
auto setDefaultMemoryResource(MemoryResourcePtr mem_resource) noexcept
    -> MemoryResourcePtr {
  MemoryResourcePtr previous;
  auto &resource = getDefaultResource();
  if (!mem_resource.isEmpty()) {
    std::scoped_lock lock(resource.mutex);
    previous = resource.resource;
    resource.resource = mem_resource;
  }
  else {
    std::scoped_lock lock(resource.mutex);
    previous = resource.resource;
    resource.resource = resource.mallocator;
  }
  return previous;
}
auto setDefaultMemoryResource(
    std::pmr::memory_resource *std_mem_resource_p) noexcept
    -> MemoryResourcePtr {
  return setDefaultMemoryResource(MemoryResourcePtr{std_mem_resource_p});
}
auto setDefaultMemoryResource(
    std::pmr::memory_resource &std_mem_resource) noexcept -> MemoryResourcePtr {
  return setDefaultMemoryResource(MemoryResourcePtr{std_mem_resource});
}

} // namespace plugins
