###################################################################################################
# Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
# This file is part of cxx_plugins project.
# License is available at https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
###################################################################################################
#
# \file    cxx_plugins_options.cmake
# \author  Andrey Ponomarev
# \date    06 May 2020
# \brief
# This file contains initialization for cached variables and options of project

option(CXX_PLUGINS_BUILD_TESTS OFF)


if (CXX_PLUGINS_BUILD_TESTS)
  enable_testing()
endif()

# we don't need to install gtest, as we embed it
option(INSTALL_GTEST OFF)