cmake_minimum_required(VERSION 3.10)
project(hexwatershed CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set C and C++ compiler flags for profiling (-pg)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR is: ${CMAKE_CURRENT_SOURCE_DIR}")

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/rapidjson)

# Compatibility for alternative directory layouts
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../src")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../external/rapidjson")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/rapidjson)
endif()

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

message(STATUS "build type = ${CMAKE_BUILD_TYPE}")

# On macOS, look for Homebrew libomp if not already specified
if(APPLE AND NOT DEFINED OpenMP_ROOT)
    if(EXISTS "/opt/homebrew/opt/libomp")
        set(OpenMP_ROOT "/opt/homebrew/opt/libomp")
    elseif(EXISTS "/usr/local/opt/libomp")
        set(OpenMP_ROOT "/usr/local/opt/libomp")
    endif()
endif()

find_package(OpenMP)

if(TARGET OpenMP::OpenMP_CXX)
    target_link_libraries(hexwatershed PRIVATE OpenMP::OpenMP_CXX)
elseif(OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# Installation directory
if(NOT DEFINED INSTALL_DIR)
    set(INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")
endif()
install(TARGETS hexwatershed RUNTIME DESTINATION "${INSTALL_DIR}")
