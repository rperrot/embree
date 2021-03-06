## ======================================================================== ##
## Copyright 2009-2016 Intel Corporation                                    ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##

SET(EMBREE_VERSION_MAJOR 2)
SET(EMBREE_VERSION_MINOR 10)
SET(EMBREE_VERSION_PATCH 0)
SET(EMBREE_VERSION_NOTE "")
SET(EMBREE_VERSION ${EMBREE_VERSION_MAJOR}.${EMBREE_VERSION_MINOR}.${EMBREE_VERSION_PATCH})
MATH(EXPR EMBREE_VERSION_NUMBER "10000*${EMBREE_VERSION_MAJOR} + 100*${EMBREE_VERSION_MINOR} + ${EMBREE_VERSION_PATCH}")
SET(CPACK_RPM_PACKAGE_RELEASE 1)

PROJECT(embree${EMBREE_VERSION_MAJOR})


CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)
CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/kernels/version.h.in"
  "${PROJECT_BINARY_DIR}/version.h"
)

IF(COMMAND cmake_policy)
  if (POLICY CMP0003)
    cmake_policy(SET CMP0003 NEW)
  endif()
  if (POLICY CMP0042)
    cmake_policy(SET CMP0042 OLD)
  endif()
ENDIF(COMMAND cmake_policy)

MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
MARK_AS_ADVANCED(EXECUTABLE_OUTPUT_PATH)
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH)

MARK_AS_ADVANCED(CMAKE_OSX_ARCHITECTURES)
MARK_AS_ADVANCED(CMAKE_OSX_DEPLOYMENT_TARGET)
MARK_AS_ADVANCED(CMAKE_OSX_SYSROOT)
MARK_AS_ADVANCED(CLEAR CMAKE_CXX_COMPILER)
MARK_AS_ADVANCED(GLUT_cocoa_LIBRARY)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/common/cmake ${CMAKE_MODULE_PATH})

INCLUDE(test)

##############################################################
# Embree configuration
##############################################################

OPTION(ENABLE_STATIC_LIB "Build Embree as a static library." OFF)
IF (ENABLE_STATIC_LIB)
  SET(EMBREE_LIB_TYPE STATIC)
  ADD_DEFINITIONS(-DENABLE_STATIC_LIB)
ELSE()
  SET(EMBREE_LIB_TYPE SHARED)
ENDIF()

OPTION(ENABLE_ISPC_SUPPORT "Build Embree with support for ISPC applications." ON)
OPTION(ENABLE_TUTORIALS    "Enable to build Embree tutorials" ON)

##############################################################################
# Configurations (add configurations also to common/cmake/embree-config-default.cmake)

OPTION(RTCORE_STAT_COUNTERS "Enables statistic counters.")

OPTION(RTCORE_RAY_MASK "Enables ray mask support.")
OPTION(RTCORE_BACKFACE_CULLING "Enables backface culling.")
OPTION(RTCORE_INTERSECTION_FILTER "Enables intersection filter callback." ON)
OPTION(RTCORE_INTERSECTION_FILTER_RESTORE "Restores previous hit when hit is filtered out." ON)
OPTION(RTCORE_IGNORE_INVALID_RAYS "Ignores invalid rays." OFF) # FIXME: enable by default?

OPTION(RTCORE_GEOMETRY_TRIANGLES "Enables support for triangle geometries." ON)
OPTION(RTCORE_GEOMETRY_QUADS "Enables support for quad geometries." ON)
OPTION(RTCORE_GEOMETRY_LINES "Enables support for line geometries." ON)
OPTION(RTCORE_GEOMETRY_HAIR "Enables support for hair geometries." ON)
OPTION(RTCORE_GEOMETRY_SUBDIV "Enables support for subdiv geometries." ON)
OPTION(RTCORE_GEOMETRY_USER "Enables support for user geometries." ON)
OPTION(RTCORE_RAY_PACKETS "Enabled support for ray packets." ON)

OPTION(RTCORE_EXPORT_ALL_SYMBOLS "Lets Embree library export all symbols.")
OPTION(RTCORE_ENABLE_RAYSTREAM_LOGGER "Enables ray stream logger.")
OPTION(RTCORE_ITT "Enable instrumentation with the ITT API." OFF)

SET(RTCORE_TASKING_SYSTEM "TBB" CACHE STRING "Selects tasking system")
SET_PROPERTY(CACHE RTCORE_TASKING_SYSTEM PROPERTY STRINGS TBB INTERNAL)

IF (RTCORE_TASKING_SYSTEM STREQUAL "TBB")
  SET(TASKING_TBB      ON )
  SET(TASKING_INTERNAL OFF)
ELSE()
  SET(TASKING_INTERNAL ON )
  SET(TASKING_TBB      OFF)
ENDIF()

IF (TASKING_INTERNAL)
  ADD_DEFINITIONS(-DTASKING_INTERNAL)
ENDIF()

IF (TASKING_TBB)
  ADD_DEFINITIONS(-DTASKING_TBB)
ENDIF()

IF (RTCORE_ITT)
  FIND_PACKAGE(ITT REQUIRED)
  ADD_DEFINITIONS(-DRTCORE_ITT)
  INCLUDE_DIRECTORIES(${ITT_INCLUDE_DIR})
ENDIF()

CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/kernels/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

##############################################################
# Create Binary Packages (uses above config options)
##############################################################
include(package)

IF (TASKING_TBB)
  FIND_PACKAGE(TBB REQUIRED)
  INCLUDE_DIRECTORIES(${TBB_INCLUDE_DIRS})
ENDIF()

##############################################################
# Compiler
##############################################################

SET(CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo")

IF (WIN32)
  IF (NOT DEFAULT_CMAKE_CONFIGURATION_TYPES_SET)
    SET(CMAKE_CONFIGURATION_TYPES "${CONFIGURATION_TYPES}" CACHE STRING "List of generated configurations." FORCE)
    SET(DEFAULT_CMAKE_CONFIGURATION_TYPES_SET ON CACHE INTERNAL "Default CMake configuration types set.")
  ENDIF()
  SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

  IF (${CMAKE_GENERATOR_TOOLSET} MATCHES "^LLVM" )
    INCLUDE(clang)
  ELSE()
    INCLUDE (msvc)
  ENDIF()

ELSE (WIN32)
  IF (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    INCLUDE (gcc)
  ELSEIF (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    INCLUDE (clang)
  ELSEIF (${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
    INCLUDE (icc)
  ELSE ()
    MESSAGE(FATAL_ERROR "Unsupported compiler: " ${CMAKE_CXX_COMPILER_ID})
  ENDIF ()

  IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "Specifies the build type." FORCE)
    SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CONFIGURATION_TYPES})
  ENDIF(NOT CMAKE_BUILD_TYPE)

  OPTION(CMAKE_VERBOSE_MAKEFILE "Enables verbose mode.")
  MARK_AS_ADVANCED(CLEAR CMAKE_VERBOSE_MAKEFILE)

ENDIF (WIN32)

##############################################################
# ISA configuration
##############################################################

IF(APPLE)
  SET(XEON_ISA "AVX2" CACHE STRING "Selects highest ISA to support.")
ELSEIF(NOT COMPILER STREQUAL "ICC")
  SET(XEON_ISA "AVX2" CACHE STRING "Selects highest ISA to support.")
ELSE()
  SET(XEON_ISA "AVX512KNL" CACHE STRING "Selects highest ISA to support.")
ENDIF ()

SET_PROPERTY(CACHE XEON_ISA PROPERTY STRINGS SSE2 SSE3 SSSE3 SSE4.1 SSE4.2 AVX AVX-I AVX2 AVX512KNL)

IF (XEON_ISA STREQUAL "SSE2")
  SET(ISA  1)
ENDIF ()

IF (XEON_ISA STREQUAL "SSE3")
  SET(ISA  2)
ENDIF ()

IF (XEON_ISA STREQUAL "SSSE3")
  SET(ISA  3)
ENDIF ()

IF (XEON_ISA STREQUAL "SSE4.1")
  SET(ISA  4)
ENDIF ()

IF (XEON_ISA STREQUAL "SSE4.2")
  SET(ISA  5)
ENDIF ()

IF (XEON_ISA STREQUAL "AVX")
  SET(ISA  6)
ENDIF ()

IF (XEON_ISA STREQUAL "AVX-I")
  SET(ISA  7)
ENDIF ()

IF (XEON_ISA STREQUAL "AVX2")
  SET(ISA  8)
ENDIF ()

IF (XEON_ISA STREQUAL "AVX512KNL")
  SET(ISA  9)
ENDIF ()

SET(TARGET_SSE2  OFF)
SET(TARGET_SSE3  OFF)
SET(TARGET_SSSE3  OFF)
SET(TARGET_SSE41  OFF)
SET(TARGET_SSE42  OFF)
SET(TARGET_AVX  OFF)
SET(TARGET_AVX2  OFF)
SET(TARGET_AVX512KNL OFF)

IF (ISA GREATER 0)
  SET(TARGET_SSE2  ON)
  SET(ISPC_TARGETS "sse2")
ENDIF ()

IF (ISA GREATER 1)
  SET(TARGET_SSE3  ON)
ENDIF ()

IF (ISA GREATER 2)
  SET(TARGET_SSSE3  ON)
ENDIF ()

IF (ISA GREATER 3)
  SET(TARGET_SSE41  ON)
  LIST(APPEND ISPC_TARGETS "sse4")
ENDIF ()

IF (ISA GREATER 4)
  SET(TARGET_SSE42  ON)
ENDIF ()

IF (ISA GREATER 5)
  SET(TARGET_AVX  ON)
  LIST(APPEND ISPC_TARGETS "avx")
ENDIF ()

IF (ISA GREATER 7)
  SET(TARGET_AVX2  ON)
  LIST(APPEND ISPC_TARGETS "avx2")
ENDIF ()

IF (ISA GREATER 8)
  SET(TARGET_AVX512KNL  ON)
  LIST(APPEND ISPC_TARGETS "avx512knl-i32x16")
ENDIF ()

INCLUDE (ispc)

##############################################################
# Search paths
##############################################################
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")
INCLUDE_DIRECTORIES_ISPC("${PROJECT_BINARY_DIR}")

##############################################################
# Output paths
##############################################################
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

##############################################################
# Directories to compile
##############################################################

ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(kernels)

IF (ENABLE_TUTORIALS)
  ADD_SUBDIRECTORY(tutorials)
  ADD_SUBDIRECTORY(tests)
ENDIF()

##############################################################
# Uninstall
##############################################################

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/common/cmake/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)

SET_PROPERTY(TARGET uninstall PROPERTY FOLDER CMakePredefinedTargets)
	
##############################################################
# Has to be last
##############################################################
INCLUDE(CPack)
