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

struct mem_block {
  void *ptr;
  size_t size;
};
