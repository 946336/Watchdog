cmake_minimum_required(VERSION 3.0)
project(Watchdog)

# ==========================================================================
# Include things

# Provides define_file_basename_for_sources
include(basename.cmake)

# ==========================================================================
# Ensure directories exist
# ==========================================================================

file(MAKE_DIRECTORY bin)
file(MAKE_DIRECTORY lib)
file(MAKE_DIRECTORY modules)

# ==========================================================================
# Setup env
# ==========================================================================

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ==========================================================================
# Set requirements and flags
# ==========================================================================

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -pedantic -g")
set(CMAKE_CC_FLAGS "-Wall -Wextra -Werror -pedantic -g")

# ==========================================================================
# Include headers
# ==========================================================================

include_directories(include)

# ==========================================================================
# Include from modules
# ==========================================================================

# include_directories(modules)

# ==========================================================================
#  Define targets
# ==========================================================================
# set_target_properties(target PROPERTIES ...)
# target_compile_features(target PRIVATE features...)

# add_executable()
# add_library()

add_executable(playground src/playground.cpp)
target_compile_features(playground PRIVATE cxx_generic_lambdas)

add_executable(tryit src/main.cpp)
target_compile_features(tryit PRIVATE cxx_lambdas)
target_compile_definitions(tryit PRIVATE WATCHDOG_DEBUG=true)

