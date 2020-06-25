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

#include "cxx_plugins/polymorphic_ref.hpp"

namespace CxxPlugins {

struct allocate {};
struct deallocate {};
struct isEqual {};

using MemoryResourceRef = PrimitivePolymorphicRef<
    TaggedValue<allocate, void *(std::size_t bytes, std::size_t alignment)>,
    TaggedValue<deallocate,
                void(void *p, std::size_t bytes, std::size_t alignment)>,
    TaggedValue<isEqual, bool(const SelfType &) const> >;

auto operator==(MemoryResourceRef const &lhs,
                MemoryResourceRef const &rhs) -> bool {
  return lhs.call<isEqual>(rhs);
}


} // namespace CxxPlugins