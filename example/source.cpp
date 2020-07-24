/*************************************************************************************************
 * Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
 * This file is part of CXX Plugins project.
 * License is available at
 * https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
 *************************************************************************************************/
/*!
 * \file    source.cpp
 * \author  Andrey Ponomarev
 * \date    21 Jul 2020
 * \brief
 * $BRIEF$
 */

#include "api.hpp"
#include "example_plugin_export.h"

#include <cxx_plugins/polymorphic_ref.hpp>

#include <iostream>

#define EXPORT extern "C" EXAMPLE_PLUGIN_EXPORT

EXPORT void(*callback_ptr)(char const* str) = nullptr;

static void log(char const* message_p) {
  if (callback_ptr != nullptr) {
    callback_ptr(message_p);
  }
}


class GraphicsEngine {
public:
  static void init() { log("Init graphics engine\n"); }
  static void update() { log("Update Graphics Engine\n"); }
  static void shutdown() { log("Shutdown Graphics Engine\n"); }
};

class DebugRenderer {
public:
  static void init() { log("Init DebugRenderer\n"); }
  static void update() { log("Update DebugRenderer\n"); }
  static void shutdown() { log("Shutdown DebugRenderer\n"); }
};

static GraphicsEngine graphics_engine = {};
static DebugRenderer debug_renderer = {};

EXPORT auto get_graphics_engine() -> SystemRef { return graphics_engine; }

EXPORT auto get_debug_renderer() -> SystemRef { return debug_renderer; }



static void initPlugin() { log("Init plugin.\n"); }

static void shutdownPlugin() { log("Shutdown plugin.\n"); }

EXPORT auto const init_plugin = initPlugin;
EXPORT auto const shutdown_plugin = shutdownPlugin;


EXPORT int const number_of_systems = 2;


#undef EXPORT