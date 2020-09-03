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

namespace CxxPlugins {

static std::mutex resource_mutex;
static ::utility::Mallocator global_mallocator = {};
static MemoryResourcePtr default_resource = &global_mallocator;

auto getDefaultMemoryResource() noexcept -> MemoryResourcePtr {
  MemoryResourcePtr result;
  {
    std::scoped_lock lock(resource_mutex);
    result = default_resource;
  }
  return result;
}
auto setDefaultMemoryResource(MemoryResourcePtr mem_resource) noexcept
    -> MemoryResourcePtr {
  MemoryResourcePtr previous;
  if (!mem_resource.isEmpty()) {
    std::scoped_lock lock(resource_mutex);
    previous = default_resource;
    default_resource = mem_resource;
  } else {
    std::scoped_lock lock(resource_mutex);
    previous = default_resource;
    default_resource = std::pmr::new_delete_resource();
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

} // namespace CxxPlugins
