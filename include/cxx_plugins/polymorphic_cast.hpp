/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev
 * This file is part of ECSFramework project.
 * License is available at
 * https://gitlab.com/andrey.ponomarev.1408/ecsframework
 *************************************************************************************************/
/*!
 * \file    polymorphic_cast.hpp
 * \author  Andrey Ponomarev
 * \date    07 Sep 2020
 * \brief
 * Defines polymorphicCast
 */
#pragma once

template<typename T, typename Poly>
T* polymorphicCast(Poly& poly) {
  if (!poly.template isA<T>())
    return nullptr;
  return static_cast<T*>(poly.data());
}

template<typename T, typename Poly>
T const* polymorphicCast(Poly const& poly) {
  if (!poly.template isA<T>())
    return nullptr;
  return static_cast<T const*>(poly.data());
}