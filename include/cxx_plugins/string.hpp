/*************************************************************************************************
 * Copyright (C) 2020 by DigiPen
 * This file is part of Material Editor project.
 *************************************************************************************************/
/*!
 * \file    string.hpp
 * \author  Andrey Ponomarev
 * \date    01 Oct 2020
 * \brief
 *
 */
#pragma once

#include "cxx_plugins/polymorphic_allocator.hpp"

#include <string>

namespace plugins {

template <typename CharT, typename Traits = std::char_traits<CharT>,
          typename Allocator = PolymorphicAllocator<CharT>>
using BasicString = std::basic_string<CharT, Traits, Allocator>;

using String = BasicString<char>;
using WString = BasicString<wchar_t>;

#if defined(__cpp_lib_char8_t)
using U8String = BasicString<char8_t>;
#endif

using U16String = BasicString<char16_t>;
using U32String = BasicString<char32_t>;

} // namespace plugins

template <typename CharT, typename Traits>
struct std::hash<plugins::BasicString<CharT, Traits>> {
  std::size_t operator()(plugins::BasicString<CharT, Traits> const
                             &str) const noexcept {
    std::basic_string_view<CharT, Traits> str_view{str};
    return std::hash<std::basic_string_view<CharT, Traits>>{}(str_view);
  }
};