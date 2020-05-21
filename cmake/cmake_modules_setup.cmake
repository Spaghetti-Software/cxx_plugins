###################################################################################################
# Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
# This file is part of cxx_plugins project.
# License is available at https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
###################################################################################################
#
# \file    cmake_modules_setup.cmake
# \author  Andrey Ponomarev
# \date    06 May 2020
# \brief
# Cmake file for setting up module paths

set(CMAKE_MODULE_PATH 
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/external/cmake-modules 
    ${CMAKE_MODULE_PATH}
)
