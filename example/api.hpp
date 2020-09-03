/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    api.hpp
 * \author  Andrey Ponomarev
 * \date    21 Jul 2020
 * \brief
 * $BRIEF$
 */
#pragma once

#include <boost/dll/alias.hpp>
#include <cxx_plugins/polymorphic_ptr.hpp>

namespace System {
struct init{};
struct update{};
struct shutdown{};
}

using SystemRef = CxxPlugins::PolymorphicPtr<
    CxxPlugins::TaggedSignature<System::init,void()>,
    CxxPlugins::TaggedSignature<System::update, void()>,
    CxxPlugins::TaggedSignature<System::shutdown, void()>
>;


using APIPluginInit = void(*)();
using APIPluginShutdown = void(*)();
using APISystemGet = SystemRef(*)();
using APISystemGetSignature = SystemRef();

template<typename T>
void polymorphicExtend(System::init, T& sys) {
  sys.init();
}


template<typename T>
void polymorphicExtend(System::update, T& sys) {
  sys.update();
}


template<typename T>
void polymorphicExtend(System::shutdown, T& sys) {
  sys.shutdown();
}


