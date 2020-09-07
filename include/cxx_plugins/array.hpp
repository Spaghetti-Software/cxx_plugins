/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev
 * This file is part of ECSFramework project.
 * License is available at
 * https://gitlab.com/andrey.ponomarev.1408/ecsframework
 *************************************************************************************************/
/*!
 * \file    array.hpp
 * \author  Andrey Ponomarev
 * \date    07 Sep 2020
 * \brief
 * Contains alias for std::array. This file is introduced just so we have all
 * containers in our library
 */
#pragma once

#include <array>

namespace plugins {
template <typename T, std::size_t Size>
//! \brief Alias for std::array
using Array = std::array<T, Size>;
} // namespace plugins