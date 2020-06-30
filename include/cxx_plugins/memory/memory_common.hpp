/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of cxx_plugins project.
 * License is available at
 *https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    memory_common.hpp
 * \author  Timur Kazhimuratov
 * \date    11 May 2020
 * \brief
 * This file contains common definitions for code related to memory management.
 *
 */
#pragma once

#include <cstddef>

namespace utility {

struct mem_block {
  void *ptr;
  std::size_t size;
};

constexpr std::size_t roundLengthToAlignment(std::size_t len, std::size_t alignment) {
  return len % alignment ? len + (alignment - len % alignment) : len;
}

}

