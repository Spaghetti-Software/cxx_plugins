/*************************************************************************************************
 * Copyright (C) 2020 by DigiPen
 * This file is part of Material Editor project.
 *************************************************************************************************/
/*!
 * \file    string_view.hpp
 * \author  Andrey Ponomarev
 * \date    01 Oct 2020
 * \brief
 *
 */
#pragma once


#include <string_view>

namespace plugins {

template <typename CharT, typename Traits = std::char_traits<CharT>>
using BasicStringView = std::basic_string_view<CharT, Traits>;

using StringView = BasicStringView<char>;
using WStringView = BasicStringView<wchar_t>;
#if defined(__cpp_lib_char8_t)
using U8StringView = BasicStringView<char8_t>;
#endif
using U16StringView = BasicStringView<char16_t>;
using U32StringView = BasicStringView<char32_t>;

} // namespace plugins