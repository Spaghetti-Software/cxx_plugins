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
#include <exception>

/*!
 * \brief Assert function.
 * \details
 * Function terminates program if both conditions are satisfied:
 * + `CXX_PLUGINS_ENABLE_ASSERT` is defined or `NDEBUG` macro is undefined
 * + `condition` is not satisfied
 */
constexpr void cxxPluginsAssert([[maybe_unused]] bool condition,
                                       [[maybe_unused]] std::string_view message) {
#if defined(CXX_PLUGINS_ENABLE_ASSERT) || !defined(NDEBUG)
  if (!condition) {
    fprintf(stderr, "Assertion failed. Message: '%*.*s'", int(message.size()),
            int(message.size()), message.data());
    fflush(stderr);
    std::abort();
  }
#endif
}

/*!
 * \brief Prints given message and terminates program.
 */
[[noreturn]] inline void
cxxPluginsUnreachable(std::string_view message) {
  fprintf(stderr, "\nUnreachable code reached. Message: '%*.*s'",
          int(message.size()), int(message.size()), message.data());
  fflush(stderr);
  std::abort();
}


inline void cxxPluginsBadArrayLength() {
#ifdef __cpp_exceptions
    throw std::bad_array_new_length();
#else
  std::fprintf(stderr,
               "Bad array length.");
  std::fflush(stderr);
  std::terminate();
#endif
}