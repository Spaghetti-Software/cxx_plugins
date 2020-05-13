/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    definitions.hpp
 * \author  Andrey Ponomarev
 * \date    13 May 2020
 * \brief
 * Contains macro/helper functions definitions for project
 */
#pragma once

#include <cstdio>
#include <cstdlib>
#include <string_view>

static constexpr void cxxPluginsAssert(bool condition,
                                       std::string_view message) {
#if defined(CXX_PLUGINS_ENABLE_ASSERT) || !defined(NDEBUG)
  if (!condition) {
    fprintf(stderr, "Assertion failed: %*.*s", int(message.size()),
            int(message.size()), message.data());
    fflush(stderr);
    std::abort();
  }
#endif
}

[[noreturn]] static inline void
cxxPluginsUnreachable(std::string_view message) {
  fprintf(stderr, "\nUnreacheable code reached. Message: '%*.*s'",
          int(message.size()), int(message.size()), message.data());
  fflush(stderr);
  std::abort();
}