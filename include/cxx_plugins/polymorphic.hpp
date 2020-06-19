/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    polymorphic.hpp
 * \author  Andrey Ponomarev
 * \author  Timur Kazhimuratovs
 * \date    21 May 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include "cxx_plugins/vtable.hpp"
#include "cxx_plugins/polymorphic_traits.hpp"

namespace impl {
  template<typename Allocator, >
  class PolymorphicImpl {
    template<typename T, typename... Tags>
    PolymorphicImpl(T&& t, Tags tags...)
  };
}