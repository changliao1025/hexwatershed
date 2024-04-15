cmake_minimum_required(VERSION 3.10)
project(hexwatershed CXX)

set(CMAKE_CXX_STANDARD 11)
# Set C and C++ compiler flags for profiling (-pg)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
message("CMAKE_CURRENT_SOURCE_DIR is = ${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Automatically include all source files in the src directory
file(GLOB_RECURSE hexwatershed_srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(hexwatershed ${hexwatershed_srcs})

set(default_build_type Debug)
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type Release)
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

message("build type = ${default_build_type}")

target_compile_options(hexwatershed PUBLIC -std=c++11)

set(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
install(TARGETS hexwatershed RUNTIME DESTINATION ${INSTALL_DIR})

find_package(OpenMP)

if (OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(RAPIDJSON_NCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/rapidjson)

include_directories(${RAPIDJSON_NCLUDE_DIR})

target_link_libraries(hexwatershed  -std=c++11)
